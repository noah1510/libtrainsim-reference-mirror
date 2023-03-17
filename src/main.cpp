#include "video.hpp"
#include "control.hpp"
#include "simulator_config.hpp"

#include <future>
#include <memory>
#include <cassert>

#include "simulator.hpp"
#include "mainMenu.hpp"

using namespace std::literals;
using namespace SimpleGFX::SimpleGL;
using namespace libtrainsim::Video;
using namespace libtrainsim::core;

int main(int argc, char* argv[]){

    #ifdef setenv
        setenv( "MESA_DEBUG", "", 0 );
        setenv( "mesa_glthread", "true", 1 );
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


/*
int main(int argc, char* argv[]){
    
    //print the individual cmd arguments to make debugging them easier
    std::cout << "command line args:" << std::endl;
    for (int i = 0; i < argc;i++){
        std::cout << argv[i] << std::endl;
    }

    //check if the libtrainsim version is high enough
    const libtrainsim::core::version required_version{0,11,0};
    assert((libtrainsim::core::lib_version >= required_version) && "libtrainsim version not high enogh!");

    //load the simulator configuration
    std::unique_ptr<configSelectionWindow> configSelection = nullptr;
    if(argc > 1){
        configSelection = std::make_unique<configSelectionWindow>(argv[1]);
    }else{
        configSelection = std::make_unique<configSelectionWindow>();
    }

    auto conf = configSelection->getConfig();
    if(conf == nullptr){
        return 100;
    }
    
}*/
