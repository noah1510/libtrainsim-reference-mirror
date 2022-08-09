#include "simulator.hpp"
#include <ctime>
#include <future>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ratio>
#include <sstream>

using namespace libtrainsim::core;

using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::base::literals;
using namespace sakurajin::unit_system::common::literals;

bool simulator::hasErrored(){
    return hasError.get();
}

simulator::~simulator(){
    hasError = true;
    //graphicsLoop.get();
    std::cout << std::endl;
    std::cout << "simulator has exited" << std::endl;
}

simulator::simulator(std::shared_ptr<libtrainsim::core::simulatorConfiguration> settings):
    track{settings->getCurrentTrack()},
    phy{settings->getCurrentTrack()},
    video{},
    statusWindow{}
{
    //load video file
    try{
        video.load(track.getVideoFilePath());
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not load video"));
    }
    
    //create the empty window
    try{
        video.createWindow(track.getName(),settings->getShaderLocation());
    }catch(const std::exception& e){
        std::throw_with_nested(std::runtime_error("Could not create simulator window"));
    }

    hasError = false;
}

bool simulator::updateImage(){
    //store the last location
    static auto last_position = track.firstLocation();
    static auto last_time = libtrainsim::core::Helper::now();
    static bool firstCall = true;

    //update the physics
    phy.tick();
    auto loc = phy.getLocation();
    
    //check if the simulator has to be closed
    if(video.reachedEndOfFile() || phy.reachedEnd()){
        end();
        return false;
    }

    //get a new frame if needed
    if (last_position < loc || firstCall){
        firstCall = false;

        //get the next frame that will be displayed
        auto frame_num = track.data().getFrame(loc);

        //if there is already a frame that is being rendered
        //then this call will buffer the furthest frame to be rendered
        video.gotoFrame(frame_num);

        //update the last position
        last_position = loc;
    }
    
    //actually render all of the windows
    try{
        libtrainsim::Video::imguiHandler::startRender();
        
        video.refreshWindow();
        statusWindow.update();
        
        libtrainsim::Video::imguiHandler::endRender();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error rendering the windows"));
    }

    //display statistics (speed, location, frametime, etc.)
    auto next_time = libtrainsim::core::Helper::now();
    
    base::time_si frametime = unit_cast(next_time-last_time, prefix::milli);
    statusWindow.appendFrametime(frametime);
    
    statusWindow.changePosition(phy.getLocation());
    statusWindow.changeEndPosition(track.lastLocation());
    
    statusWindow.setAcceleration(phy.getAcceleration());
    statusWindow.setSpeedLevel(Speedlevel);
    statusWindow.setVelocity(phy.getVelocity());

    last_time = next_time;

    return false;
}

void simulator::emergencyBreak(){
    phy.emergencyBreak();
}

void simulator::end(){
    hasError = true;
}

void simulator::serial_speedlvl(libtrainsim::core::input_axis Slvl){
    Speedlevel = Slvl;
    phy.setSpeedlevel(Slvl);    
}
