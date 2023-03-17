#include "video.hpp"
#include "control.hpp"
#include "simulator_config.hpp"

#include <future>
#include <memory>
#include <cassert>

#include "simulator.hpp"

using namespace std::literals;
using namespace SimpleGFX::SimpleGL;
using namespace libtrainsim::Video;
using namespace libtrainsim::core;

int main(int argc, char* argv[]){

    #ifdef setenv
        setenv( "MESA_DEBUG", "", 0 );
    #endif

    auto app = Gtk::Application::create("thm.bahn_simulator.reference");
    if(! app->register_application()){
        std::cerr << "could not register app!" << std::endl;
        return 1;
    }

    std::shared_ptr<simulatorConfiguration> conf = nullptr;
    try{
        conf = std::make_shared<simulatorConfiguration>("data/production_data/simulator.json");
    }catch(const std::exception& e){
        GLHelper::print_exception(e);
        return 1;
    }

    std::unique_ptr<mainMenu> menu = nullptr;

    try{
        menu = std::make_unique<mainMenu>(conf);
    }catch(const std::exception& e){
        GLHelper::print_exception(e);
        return 2;
    }

    app->add_window(*menu);
    menu->set_visible(true);

    return app->run(argc, argv);
}


/*int main(int argc, char* argv[]){
    
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
    imguiHandler::init("libtrainsim reference implementation");

    //load the simulator configuration
    std::unique_ptr<configSelectionWindow> configSelection = nullptr;
    if(argc > 1){
        configSelection = std::make_unique<configSelectionWindow>(argv[1]);
    }else{
        configSelection = std::make_unique<configSelectionWindow>();
    }
    
    do{
        imguiHandler::startRender();
            
        configSelection->draw();
        
        imguiHandler::endRender();

        if(imguiHandler::shouldTerminate()){
            return 0;
        }
    }while(!configSelection->closeWindow());

    auto conf = configSelection->getConfig();
    if(conf == nullptr){
        return 100;
    }
    
    std::unique_ptr<simulator> sim;
    bool loadingSimulator = false;
    std::unique_ptr<mainMenu> menu = std::make_unique<mainMenu>(conf);
    
    while(!imguiHandler::shouldTerminate()){
        if(!loadingSimulator){
            imguiHandler::startRender();
            
            menu->draw();
            loadingSimulator = menu->shouldStart();
            
            imguiHandler::endRender();
                
            
        }else{
            //display the loading screen before catually starting the load
            imguiHandler::startRender();
                
                ImGui::Text("Loading simulator...");
            
            imguiHandler::endRender();
            
            //start loading the simulator during the next render cycle
            //while the code is waiting for the load to finish the loading screen is being displayed
            imguiHandler::startRender();
                
                ImGui::Text("Loading simulator...");
                
                try{
                    //clear all gl errors before loading the sim
                    int glErrorCode;
                    while((glErrorCode = glGetError()) != GL_NO_ERROR){
                        std::cout << "GL Error found: " << imguiHandler::decodeGLError(glErrorCode) << std::endl;
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
            
            imguiHandler::endRender();
            
            //update the simulator in the current thread
            while(!sim->hasErrored()){
                try{
                    sim->update();
                }catch(const std::exception& e){
                    libtrainsim::core::Helper::print_exception(e);
                    sim->end();
                };

            };
            
            sim.reset();
            loadingSimulator = false;
            menu = std::make_unique<mainMenu>(conf);
            
        }
        
    }

    return 0;
}*/
