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

    while(!sim->hasErrored() && exitCode == 0){
        switch (cv::waitKey(1)){
            case('w'):
                sim->accelerate();
                break;
            case('s'):
                sim->decellerate();
                break;
            case(27):
                std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
                exitCode = 1;
                break;
            default:
                break;
        }
    };

    //all windows will be closed to prevent memory leaks
    cv::destroyAllWindows();

    return exitCode;
}
