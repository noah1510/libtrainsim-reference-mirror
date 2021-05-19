#include "video.hpp"
#include "control.hpp"
#include "track_configuration.hpp"
#include <future>
#include <memory>

#include "simulator.hpp"
#include "control.hpp"

int main(int argc, char **argv){
    int exitCode = 0;
    
    std::cout << "command line args:" << std::endl;
    for (int i = 0; i < argc;i++){
        std::cout << argv[i] << std::endl;
    }

    //check if singeltons are running
    std::cout << libtrainsim::video::hello() << std::endl;
    libtrainsim::video::setBackend(libtrainsim::opencv);
    std::cout << libtrainsim::control::hello() << std::endl;

    const auto track = libtrainsim::core::Track(argc > 1 ? argv[1] : "data/production_data/Track.json");
    if(!track.isValid()){
        std::cerr << "track data not valid" << std::endl;
        return 100;
    }
    
    std::cout << "first location" << track.firstLocation() << "; last location:" << track.lastLocation() << std::endl;
    auto sim = std::make_unique<simulator>(track);

    while(!sim->hasErrored()){
        for(unsigned int i = 0; i < 10 && exitCode == 0;i++){
            switch(libtrainsim::control::getCurrentAction()){
                case(libtrainsim::core::ACTION_ACCELERATE):
                    sim->accelerate();
                    break;
                case(libtrainsim::core::ACTION_BREAK):
                    sim->decellerate();
                    break;
                case(libtrainsim::core::ACTION_CLOSE):
                    std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
                    sim->end();
                    exitCode = 1;
                    break;
                default:
                    break;
            }
        }
    };

    return 0;
}
