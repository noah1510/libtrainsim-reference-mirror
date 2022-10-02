#include "video.hpp"
#include "control.hpp"
#include "simulator_config.hpp"

#include <future>
#include <memory>
#include <cassert>

#include "simulator.hpp"

int main(int argc, char **argv){
    
    //set a flag to display opengl errors on linux
    #ifdef setenv
        setenv( "MESA_DEBUG", "", 0 );
        setenv( "mesa_glthread", "true", 1 );
    #endif
    
    //print the individual cmd arguments to make debugging them easier
    std::cout << "command line args:" << std::endl;
    for (int i = 0; i < argc;i++){
        std::cout << argv[i] << std::endl;
    }

    //check if the libtrainsim version is high enough
    const libtrainsim::core::version required_version{0,11,0};
    assert((libtrainsim::core::lib_version >= required_version) && "libtrainsim version not high enogh!");

    //load the simulator configuration
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;
    try{
        std::filesystem::path config_loc = argc > 1 ? argv[1] : "data/production_data/simulator.json";
        conf = std::make_shared<libtrainsim::core::simulatorConfiguration>(config_loc, true);
    }catch(const std::exception& e){
        libtrainsim::core::Helper::print_exception(e);
        return 100;
    }
   
    //load the input handler with serial input if available
    std::shared_ptr<libtrainsim::control::input_handler> input;
    try{
        input = std::make_shared<libtrainsim::control::input_handler>(conf->getSerialConfigLocation());
    }catch(const std::exception& e){
        libtrainsim::core::Helper::print_exception(e);
        return 100;
    }
    
    std::unique_ptr<simulator> sim;
    while(!input->closingFlag()){
        if(sim == nullptr){
            input->update();
            //output main menu
            libtrainsim::Video::imguiHandler::startRender();
                ImGui::Begin("Main Menu");
                
                    auto trackCount = conf->getTrackCount();
                    int selectedTrackID = static_cast<int>(conf->getCurrentTrackID());
                    for(uint64_t i = 0; i < trackCount;i++){
                        const auto& track = conf->getTrack(i);
                        ImGui::RadioButton(track.getName().c_str(),&selectedTrackID,i);
                    }
                    
                    if(ImGui::SmallButton("Start Simulator")){
                        try{
                            conf->selectTrack(selectedTrackID);
                            sim = std::make_unique<simulator>(conf);
                        }catch(const std::exception& e){
                            libtrainsim::core::Helper::print_exception(e);
                        }
                    }
                
                ImGui::End();
            libtrainsim::Video::imguiHandler::endRender();
            
        }else{
            //update the output image in the current thread
            while(!sim->hasErrored()){
                input->update();
                sim->serial_speedlvl(input->getSpeedAxis());
                    
                if(input->emergencyFlag()){
                    sim->emergencyBreak();
                }
                
                if(input->closingFlag()){
                    std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
                    sim->end();
                }
                sim->updateImage();
            };
            
            sim.reset();
        }
        
        
    }
    
    input.reset();

    return 0;
}
