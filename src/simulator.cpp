#include "simulator.hpp"
#include <future>
#include <thread>
#include <chrono>

bool simulator::hasErrored(){
    return hasError.get();
}

simulator::~simulator(){
    hasError = true;
    graphicsLoop.get();
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
    auto fps = libtrainsim::video::getFPS();
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
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
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