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
    graphicsLoop.get();
    std::cout << std::endl;
    std::cout << "simulator has exited" << std::endl;
}

simulator::simulator(const Track& dat):track{dat},phy{libtrainsim::physics(dat)}{
    
    //load video file
    try{
        libtrainsim::video::load(track.getVideoFilePath());
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not load video"));
    }

    //creating the window used to display stuff
    try{
        libtrainsim::video::createWindow(track.getName());
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not create window"));
    }

    graphicsLoop = std::async([&](){
        auto lastLoop = libtrainsim::physics::now();
        std::cout << "stated graphics loop" << std::endl;
        while(!hasError){
            auto nextTime = libtrainsim::physics::now();
            if(nextTime-lastLoop > std::chrono::milliseconds(1)){
                updateImage();
                lastLoop = nextTime;
            }
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        }
    });

    hasError = false;
}

bool simulator::updateImage(){
    //store the last location
    static auto last_position = track.firstLocation();
    static auto last_time = libtrainsim::physics::now();
    static bool firstCall = true;

    phy.tick();
    auto loc = phy.getLocation();
    
    if(libtrainsim::video::reachedEndOfFile() || phy.reachedEnd()){
        end();
        return false;
    }

    //get a new frame if needed
    if (last_position < loc || firstCall){
        firstCall = false;

        //get the next frame that will be displayed
        auto frame_num = track.data().getFrame(loc);

        libtrainsim::video::gotoFrame(frame_num);

        //update the last position
        last_position = loc;
    }else{
        libtrainsim::video::refreshWindow();
    }

    //display statistics (speed, location, frametime, etc.)
    auto next_time = libtrainsim::physics::now();

    base::time_si frametime = unit_cast(next_time-last_time, prefix::milli);
    std::cout << "acceleration:" << std::setiosflags(std::ios::fixed) << std::setprecision(2) << phy.getAcceleration() << ";current Speedlevel:" << Speedlevel.get() << "; current velocity:" << phy.getVelocity() << "; current location:" << phy.getLocation() << " / " << track.lastLocation() << "; frametime:" << frametime.value << "ms; \r" << std::flush;

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
