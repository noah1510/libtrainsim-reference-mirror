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
    if(!dat.isValid()){
        std::cout << "ERROR::SIMULATOR::NON_VALID_TRACK" << std::endl;
        return;
    }

    if(!libtrainsim::video::load(dat.getVideoFilePath())){
        std::cout << "ERROR::SIMULATOR::COULD_NOT_LOAD_VIDEO" << std::endl;
        return;
    }

    //creating the window used to display stuff
    libtrainsim::video::createWindow(window_name);

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

void simulator::accelerate(){
    Speedlevel += 0.1;
    if(abs(Speedlevel.get()) < 0.07){Speedlevel = 0.0;};
    phy.setSpeedlevel(Speedlevel);
}

void simulator::decellerate(){
    Speedlevel -= 0.1;
    if(abs(Speedlevel.get()) < 0.07){Speedlevel = 0.0;};
    phy.setSpeedlevel(Speedlevel);
}

void simulator::end(){
    hasError = true;
}

void simulator::serial_speedlvl(libtrainsim::core::input_axis Slvl){
    Speedlevel = Slvl;
    if (Speedlevel == -1.0){
        while(phy.getVelocity() > 0.0_mps){
            phy.setSpeedlevel(-1.0);
        }
    }else{
        phy.setSpeedlevel(Speedlevel);
    }
    
}