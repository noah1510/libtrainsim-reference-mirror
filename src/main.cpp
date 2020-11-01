#include "opencv2/highgui.hpp"
#include "video.hpp"
#include "control.hpp"
#include "track_configuration.hpp"
#include <future>
#include <memory>

#include "simulator.hpp"

int main(){
    int exitCode = 0;

    //check if singeltons are running
    std::cout << libtrainsim::video::hello() << std::endl;
    std::cout << libtrainsim::control::hello() << std::endl;

    const auto track = libtrainsim::core::Track("data/production_data/Track.json");
    if(!track.isValid()){
        std::cerr << "track data not valid" << std::endl;
        return 100;
    }
    
    //libtrainsim::video::setBackend(cv::CAP_FFMPEG);
    
    std::cout << "first location" << track.firstLocation() << "; last location:" << track.lastLocation() << std::endl;
    auto sim = std::make_unique<simulator>(track);

    while(!sim->hasErrored()){
        for(unsigned int i = 0; i < 10 && exitCode == 0;i++){
            switch (cv::waitKey(1)){
                case('w'):
                    sim->accelerate();
                    break;
                case('s'):
                    sim->decellerate();
                    break;
                case(27):
                    std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
                    sim->end();
                    break;
                default:
                    break;
            }
        }
    };

    //all windows will be closed to prevent memory leaks
    cv::destroyAllWindows();

    return 0;
}
