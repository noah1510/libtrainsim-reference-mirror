#include "simulator.hpp"
#include <ctime>
#include <future>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ratio>
#include <sstream>

using namespace libtrainsim::core;

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
    double frametime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(next_time-last_time).count())/1000.0;
    std::cout << "acceleration:" << std::setiosflags(std::ios::fixed) << std::setprecision(2) << acceleration << "m/s^2; current velocity:" << phy.getVelocity() << "m/s; current location:" << phy.getLocation() << "m / " << track.lastLocation() << "m; frametime:" << frametime << "ms; " << 1000.0 / frametime << " fps\r" << std::flush;
    
    last_time = next_time;

    return false;
}

void simulator::accelerate(){
    acceleration = track.train().clampAcceleration(acceleration + 0.1);
    if(std::abs(acceleration) < 0.07){acceleration = 0.0;};
    phy.setAcelleration(acceleration);
}

void simulator::decellerate(){
    acceleration = track.train().clampAcceleration(acceleration - 0.1);
    if(std::abs(acceleration) < 0.07){acceleration = 0.0;};
    phy.setAcelleration(acceleration);
}

void simulator::end(){
    hasError = true;
}
