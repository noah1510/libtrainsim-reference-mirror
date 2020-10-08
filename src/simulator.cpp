#include "simulator.hpp"
#include <ctime>
#include <future>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ratio>

bool simulator::hasErrored(){
    return hasError.get();
}

simulator::~simulator(){
    hasError = true;
    graphicsLoop.get();
    speedLoop.get();
    std::cout << "simulator has exited" << std::endl;
}

simulator::simulator(std::filesystem::path URI){
    //load video file
    if(!libtrainsim::video::load(URI)){
        std::cout << "ERROR::LIBTRAINSIM::COULD_NOT_LOAD_VIDEO" << std::endl;
        hasError = true;
        return;
    }

    //check if it was loaded by getting the FPS
    auto fps = libtrainsim::video::getVideoProperty(cv::CAP_PROP_FPS);
    std::cout << "the video has " << fps << "FPS" << std::endl;

    //creating the window used to display stuff
    cv::namedWindow(window_name, cv::WINDOW_NORMAL);

    //Display the first frame
    auto frame = libtrainsim::video::getNextFrame();
    if (!frame.empty()){
        cv::imshow(window_name, frame); 
        currentFrame++;
    };

    graphicsLoop = std::async([&](){
        unsigned int lastFrame = 0;
        std::cout << "stated graphics loop" << std::endl;
        while(!hasError){
            if (lastFrame == 0 || currentFrame.get() > lastFrame){
                if(updateImage()){hasError = true;};
                lastFrame++;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });

    //This thread handles the advancing of frames based on the current speed
    speedLoop = std::async([&](){
        std::cout << "stated speed loop" << std::endl;

        //save when the last advancement was
        auto lastTime = std::chrono::system_clock::now();

        while(!hasError){

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            //save the speed in a local var to use the lock less
            auto currentSpeed = speed.get();
            std::chrono::duration<double, std::milli> deltaT = std::chrono::system_clock::now() - lastTime;

            //check if the train is moving
            if (currentSpeed > 0.5f){

                //get the time that should have passed since the last frame
                auto frametime = std::chrono::duration(std::chrono::milliseconds(static_cast<int>(1000.0f/currentSpeed)));
                if(deltaT.count() > frametime.count()){

                    //If enough time has passed advance one frame and update the time
                    nextFrame();
                    lastTime = std::chrono::system_clock::now();
                }
            }
            if (currentSpeed < 0.5f){
                lastTime = std::chrono::system_clock::now();
            }

        }
    });

}

void simulator::nextFrame(){
    currentFrame++;
}

bool simulator::updateImage(){
    //get the next frame that will be displayed
    auto frame = libtrainsim::video::getNextFrame();

    //if it is empty the end of the video was reached and the program has to quit
    if (frame.empty()){
        std::cerr << "got empty frame" << std::endl;
        return true;
    }

    //display the frame in the video
    cv::imshow(window_name, frame);

    return false;
}

void simulator::accelerate(){
    if(speed.get() < 60.0f){
        speed += 0.2f;
    }
    if(speed > 59.9f){
        speed.set(60.0);
    }
}

void simulator::decellerate(){
    if(speed.get() > 0.0f){
        speed += -1.0f;
    }
    if(speed < 0.1f){
        speed.set(0.0f);
    }
}