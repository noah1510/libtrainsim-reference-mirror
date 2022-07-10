#include "video.hpp"
#include "control.hpp"
#include "simulator_config.hpp"

#include <future>
#include <memory>
#include <cassert>

#include "simulator.hpp"

int main(int argc, char **argv){
    int exitCode = 0;

    std::cout << "command line args:" << std::endl;
    for (int i = 0; i < argc;i++){
        std::cout << argv[i] << std::endl;
    }

    //check if the libtrainsim version is high enough
    const libtrainsim::core::version required_version{0,10,0};
    assert((libtrainsim::core::lib_version >= required_version) && "libtrainsim version not high enogh!");

    libtrainsim::video::setBackend(libtrainsim::Video::VideoBackends::ffmpeg_SDL2);

    std::optional<libtrainsim::core::simulatorConfiguration> conf;
    try{
        std::filesystem::path config_loc = argc > 1 ? argv[1] : "data/production_data/simulator.json";
        conf = std::make_optional<libtrainsim::core::simulatorConfiguration>(config_loc, true);
        
    }catch(const std::exception& e){
        libtrainsim::core::Helper::print_exception(e);
        return 100;
    }
   
    std::optional<libtrainsim::control::input_handler> input;
    try{
        input = std::make_optional<libtrainsim::control::input_handler>(conf->getSerialConfigLocation());
    }catch(const std::exception& e){
        libtrainsim::core::Helper::print_exception(e);
        return 100;
    }

    auto track = conf->getCurrentTrack();

    std::cout << "first location" << track.firstLocation() << "; last location:" << track.lastLocation() << std::endl;
    
    std::unique_ptr<simulator> sim;
    try{
        sim = std::make_unique<simulator>(track);
    }catch(const std::exception& e){
        libtrainsim::core::Helper::print_exception(e);
        return -1;
    }

    while(!sim->hasErrored()){
        for(unsigned int i = 0; i < 10 && exitCode == 0;i++){
            
            input->update();
            sim->serial_speedlvl(input->getSpeedAxis());
            
            if(input->emergencyFlag()){
                sim->emergencyBreak();
            }
            
            if(input->closingFlag()){
                std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
                sim->end();
                exitCode = 1;
            }
            
        }
    };

    return 0;
}
