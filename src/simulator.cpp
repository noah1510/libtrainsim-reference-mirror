#include "simulator.hpp"

using namespace libtrainsim::core;

using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::base::literals;
using namespace sakurajin::unit_system::common::literals;
using namespace std::literals;

simulator::~simulator(){
    hasError = true;
    
    std::cout << std::endl;
    std::cout << "closing the simulator" << std::endl;
    
    std::cout << "   waiting for physics thread to end" << std::endl;
    if(physicsLoop.valid()){
        physicsLoop.wait();
        physicsLoop.get();
    }
    std::cout << "   destroying snowfx" << std::endl;
    snow.reset();
    std::cout << "   destroying status window" << std::endl;
    statusWindow.reset();
    std::cout << "   destroying videoManager" << std::endl;
    video.reset();
    std::cout << "   destroying physics" << std::endl;
    phy.reset();
    
    std::cout << "simulator has exited" << std::endl;
}

simulator::simulator(std::shared_ptr<libtrainsim::core::simulatorConfiguration> settings):track{settings->getCurrentTrack()}{
    //load the physics
    try{
        phy = std::make_unique<libtrainsim::physics>(settings->getCurrentTrack());
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error initializing physics"));
    }
    
    //load video file
    try{
        video = std::make_unique<libtrainsim::Video::videoManager>();
        video->load(track.getVideoFilePath());
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not load video"));
    }
    
    //create the empty window
    try{
        video->createWindow(track.getName(),settings->getShaderLocation(), settings->getTextureLocation());
        video->addTexture(libtrainsim::Video::imguiHandler::getDarkenTexture(20));
    }catch(const std::exception& e){
        std::throw_with_nested(std::runtime_error("Could not create simulator window"));
    }
    
    //load the status display
    try{
        statusWindow = std::make_unique<libtrainsim::extras::statusDisplay>();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create status window"));
    }
    
    //create the snow fx layer
    try{
        snow = std::make_unique<libtrainsim::extras::snowFx>(settings->getShaderLocation(),settings->getShaderLocation()/"../extras/snowFx");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not load snow fx layer"));
    }
    
    //overly the snow over the video
    try{
        auto tex = snow->getOutputTexture();
        video->addTexture(tex);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not attach snowflake texture to video class"));
    }

    hasError = false;
    
    //start the physics and video decode updates in a separate thread
    physicsLoop = std::async(std::launch::async, [&](){
        auto last_time = libtrainsim::core::Helper::now();
        
        do{
            //update the physics
            phy->tick();
            auto loc = phy->getLocation();
            
            //check if the simulator has to be closed
            if(video->reachedEndOfFile() || phy->reachedEnd()){
                end();
                return;
            }

            //get the next frame that will be displayed
            auto frame_num = track.data().getFrame(loc);

            //if there is already a frame that is being rendered
            //then this call will buffer the furthest frame to be rendered
            video->gotoFrame(frame_num);
            
            if(libtrainsim::core::Helper::now() - last_time < 5ms){
                std::this_thread::sleep_until(last_time + 5ms);
            }
            last_time = libtrainsim::core::Helper::now();
            
        }while(!hasErrored());
    });
}

bool simulator::updateImage(){
    //store the last last_time
    static auto last_time = libtrainsim::core::Helper::now();
    
    //actually render all of the windows
    try{
        libtrainsim::Video::imguiHandler::startRender();
        
        snow->updateTexture();
        video->refreshWindow();
        statusWindow->update();
        
        libtrainsim::Video::imguiHandler::endRender();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error rendering the windows"));
    }

    //display statistics (speed, location, frametime, etc.)
    auto next_time = libtrainsim::core::Helper::now();
    
    base::time_si frametime = unit_cast(next_time-last_time, prefix::milli);
    statusWindow->appendFrametime(frametime);
    
    auto renderTimes = video->getNewRendertimes();
    if(renderTimes.has_value()){
        for(auto time:renderTimes.value()){
            statusWindow->appendRendertime(time);
        }
    }
    
    statusWindow->changePosition(phy->getLocation());
    statusWindow->changeEndPosition(track.lastLocation());
    
    statusWindow->setAcceleration(phy->getAcceleration());
    statusWindow->setSpeedLevel(Speedlevel);
    
    auto vel = phy->getVelocity();
    snow->updateTrainSpeed(vel);
    statusWindow->setVelocity(vel);

    while(Helper::now()-last_time < 8ms){
        std::this_thread::sleep_until(last_time+8ms);
    }
    
    last_time = Helper::now();

    return false;
}

void simulator::emergencyBreak(){
    phy->emergencyBreak();
}

void simulator::end(){
    std::scoped_lock lock{errorMutex};
    hasError = true;
}

bool simulator::hasErrored(){
    std::shared_lock lock{errorMutex};
    return hasError;
}

void simulator::serial_speedlvl(libtrainsim::core::input_axis Slvl){
    Speedlevel = Slvl;
    phy->setSpeedlevel(Slvl);    
}
