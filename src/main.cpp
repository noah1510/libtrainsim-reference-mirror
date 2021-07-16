#include "video.hpp"
#include "control.hpp"
#include "track_configuration.hpp"
#include <future>
#include <memory>
#include <cassert>

#include "simulator.hpp"
#include "control.hpp"

int main(int argc, char **argv){
    int exitCode = 0;

    std::cout << "command line args:" << std::endl;
    for (int i = 0; i < argc;i++){
        std::cout << argv[i] << std::endl;
    }

    //check if the libtrainsim version is high enough
    const libtrainsim::core::version required_version{0,8,0};
    assert((libtrainsim::core::lib_version >= required_version) && "libtrainsim version not high enogh!");

    //check if singeltons are running
    std::cout << libtrainsim::video::hello() << std::endl;

    libtrainsim::video::setBackend(libtrainsim::Video::VideoBackends::ffmpeg_SDL2);

    libtrainsim::control::input_handler input{};
    std::cout << input.hello() << std::endl;

    const auto track = libtrainsim::core::Track(argc > 1 ? argv[1] : "data/production_data/Track.json");
    if(!track.isValid()){
        std::cerr << "track data not valid" << std::endl;
        return 100;
    }

    std::cout << "first location" << track.firstLocation() << "; last location:" << track.lastLocation() << std::endl;
    auto sim = std::make_unique<simulator>(track);

    while(!sim->hasErrored()){
        for(unsigned int i = 0; i < 10 && exitCode == 0;i++){

            auto command = input.getKeyFunction();

            if(command == "ACCELERATE"){
                sim->accelerate();
            }

            if(command == "BREAK"){
                sim->decellerate();
            }

            if(command == "CLOSE"){
                std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
                sim->end();
                exitCode = 1;
            }
        }
    };

    return 0;
}
