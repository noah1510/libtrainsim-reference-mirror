#include "video.hpp"
#include "simulator_config.hpp"

#include <future>
#include <memory>
#include <cassert>

#include "simulator.hpp"

using namespace std::literals;

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
    
    //init the imgui handler
    libtrainsim::Video::imguiHandler::init("libtrainsim reference implementation");

    //load the simulator configuration
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;
    try{
        std::filesystem::path config_loc = argc > 1 ? argv[1] : "data/production_data/simulator.json";
        conf = std::make_shared<libtrainsim::core::simulatorConfiguration>(config_loc, true);
    }catch(const std::exception& e){
        libtrainsim::core::Helper::print_exception(e);
        return 100;
    }
    
    std::unique_ptr<simulator> sim;
    bool loadingSimulator = false;
    std::unique_ptr<mainMenu> menu = std::make_unique<mainMenu>(conf);
    
    while(!libtrainsim::Video::imguiHandler::shouldTerminate()){
        if(!loadingSimulator){
            libtrainsim::Video::imguiHandler::startRender();
            
            menu->draw();
            loadingSimulator = menu->shouldStart();
            
            libtrainsim::Video::imguiHandler::endRender();
                
            
        }else{
            //display the loading screen before catually starting the load
            libtrainsim::Video::imguiHandler::startRender();
                
                ImGui::Text("Loading simulator...");
            
            libtrainsim::Video::imguiHandler::endRender();
            
            //start loading the simulator during the next render cycle
            //while the code is waiting for the load to finish the loading screen is being displayed
            libtrainsim::Video::imguiHandler::startRender();
                
                ImGui::Text("Loading simulator...");
                
                try{
                    //clear all gl errors before loading the sim
                    int glErrorCode;
                    while((glErrorCode = glGetError()) != GL_NO_ERROR){
                        std::cout << "GL Error found: " << libtrainsim::Video::imguiHandler::decodeGLError(glErrorCode) << std::endl;
                    }
                    
                    menu->finishTrackLoad();
                    auto selectedTrackID = menu->getSelectedTrack();
                    auto [stopBegin, stopEnd] = menu->getStopIDs();
                    menu.reset();
                    menu = nullptr;
                    
                    //actually select the selected track
                    conf->selectTrack(selectedTrackID);
                    const auto& stops = conf->getCurrentTrack().getStations();
                    conf->getTrack(selectedTrackID).setFirstLocation(stops[stopBegin].position());
                    conf->getTrack(selectedTrackID).setLastLocation(stops[stopEnd].position());
                    sim = std::make_unique<simulator>(conf);
                }catch(const std::exception& e){
                    libtrainsim::core::Helper::print_exception(e);
                    break;
                }
            
            libtrainsim::Video::imguiHandler::endRender();
            
            //update the simulator in the current thread
            while(!sim->hasErrored()){
                sim->update();
            };
            
            sim.reset();
            loadingSimulator = false;
            menu = std::make_unique<mainMenu>(conf);
            
        }
        
    }

    return 0;
}
