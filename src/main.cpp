#include "opencv2/highgui.hpp"
#include "video.hpp"
#include "control.hpp"
#include <future>
#include <memory>

#include "simulator.hpp"

int main( int argc, char **argv){
    int exitCode = 0;
    if (argc == 0){
        return 100;
    }

    //check if singeltons are running
    std::cout << libtrainsim::video::hello() << std::endl;
    std::cout << libtrainsim::control::hello() << std::endl;

    auto sim = std::make_unique<simulator>("data/hq/U6_vp9.mkv");

    auto waitForEscape = std::async([&](){
        while(!sim->hasErrored()){
            // if esc was pressed the window should be closed
            if (cv::waitKey(5) == 27){
                std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
                break;
            }
        }
    });

    auto handleAcceleration = std::async([&](){
        while(!sim->hasErrored()){
            // if esc was pressed the window should be closed
            if (cv::waitKey(5) == 'a'){
                sim->nextFrame();
            }
        }
    });

    waitForEscape.get();
    handleAcceleration.get();

    //all windows will be closed to prevent memory leaks
    cv::destroyAllWindows();

    return exitCode;
}
