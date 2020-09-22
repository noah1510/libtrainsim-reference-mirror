#include "video.hpp"
#include "control.hpp"

int main( int argc, char **argv){
    int exitCode = 0;
    if (argc == 0){
        return 100;
    }

    //load video file
    if(!libtrainsim::video::load("data/hq/U6_vp9.mkv")){
        std::cout << "ERROR::LIBTRAINSIM::COULD_NOT_LOAD_VIDEO" << std::endl;
        return -30;
    }

    //check if it was loaded by getting the FPS
    std::cout << "the video has " << libtrainsim::video::getFPS() << "FPS" << std::endl;

    //creating the window used to display stuff
    std::string window_name = " bahn_simulator";
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

    //all the displaying happens here
    while(true){
        //get the next frame that will be displayed
        auto frame = libtrainsim::video::getNextFrame();

        //if it is empty the end of the video was reached and the program has to quit
        if (frame.empty()){
            std::cerr << "got empty frame" << std::endl;
            break;
        }

        //display the frame in the video
        cv::imshow(window_name, frame);
        
        // if esc was pressed the window should be closed
        if (cv::waitKey(10) == 27){
            std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
            break;
        }
    }

    //all windows will be closed to prevent memory leaks
    cv::destroyAllWindows();

    return exitCode;
}
