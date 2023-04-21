#include "video.hpp"
#include "control.hpp"
#include "simulator_config.hpp"

#include <future>
#include <memory>
#include <cassert>

#include "simulator.hpp"
#include "mainMenu.hpp"
#include "appLauncher.hpp"
#include "loggerWindow.hpp"

using namespace std::literals;
using namespace SimpleGFX::SimpleGL;
using namespace libtrainsim::Video;
using namespace libtrainsim::core;

const std::string appName = "thm.bahn_simulator.reference";

class mainApp : public SimpleGFX::SimpleGL::appLauncher{
    private:
        std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;
        std::unique_ptr<mainMenu> menu;
        std::shared_ptr<SimpleGFX::SimpleGL::loggerWindow> loggerWin;

        void prepare() override{

           try{
                loggerWin = SimpleGFX::SimpleGL::loggerWindow::createManaged(SimpleGFX::loggingLevel::detail);
                conf->getLogger()->addExtraLogger(loggerWin);
                add_window(*loggerWin);
                loggerWin->set_visible(true);
            }catch(...){
                std::throw_with_nested(std::runtime_error("could not create logger window"));
            }

        }

        void load() override{
            try{
                menu = std::make_unique<mainMenu>(conf);
                add_window(*menu);
                menu->set_visible(true);
            }catch(...){
                conf->getLogger()->logCurrrentException(true);
                std::throw_with_nested(std::runtime_error("could not create main menu"));
            }

            loadFinished = true;
        }
    public:
        mainApp() : appLauncher(appName, Gio::Application::Flags::NONE, true){
            try{
                conf = std::make_shared<simulatorConfiguration>("data/production_data/simulator.json", true, appName, true);
            }catch(...){
                std::throw_with_nested(std::runtime_error("could not load configuration"));
            }
        }
};

int main(int argc, char* argv[]){

    #ifdef setenv
        setenv( "MESA_DEBUG", "", 0 );
        setenv( "mesa_glthread", "true", 1 );
    #endif

    std::shared_ptr<mainApp> app = nullptr;

    try{
        app = std::make_shared<mainApp>();
    }catch(const std::exception& e){
        libtrainsim::core::Helper::printException(e);
        return 100;
    }

    return app->launch(argc, argv);
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
