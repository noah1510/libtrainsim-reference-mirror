#include "mainWindow.hpp"

using namespace std::literals;
using namespace SimpleGFX::SimpleGL;
using namespace libtrainsim::Video;
using namespace libtrainsim::core;

class mainApp;

const std::string               appName     = "thm.bahn_simulator.reference";
static std::shared_ptr<mainApp> appInstance = nullptr;

class mainApp : public SimpleGFX::SimpleGL::appLauncher {
  private:
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;
    std::unique_ptr<mainWindow>                                menu;
    std::shared_ptr<SimpleGFX::SimpleGL::loggerWindow>         loggerWin;

    void load() override {
        try {
            // loggerWin = SimpleGFX::SimpleGL::loggerWindow::createManaged(SimpleGFX::loggingLevel::detail);
            // conf->getLogger()->addExtraLogger(loggerWin);
            // add_window(*loggerWin);
            // loggerWin->set_visible(true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error("could not create logger window"));
        }

        try {
            menu = std::make_unique<mainWindow>(conf, appInstance);
            add_window(*menu);
            menu->set_visible(true);
            menu->present();
        } catch (...) {
            conf->getLogger()->logCurrrentException(true);
            std::throw_with_nested(std::runtime_error("could not create main menu"));
        }
    }

    void on_startup() override { appLauncher::on_startup(); }

  public:
    mainApp()
        : appLauncher(appName, Gio::Application::Flags::NONE, false) {
        try {
            conf = std::make_shared<simulatorConfiguration>("data/production_data/simulator.json", true, appName, true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error("could not load configuration"));
        }

        auto quit = Gio::SimpleAction::create("quit");
        quit->signal_activate().connect([this](const Glib::VariantBase&) { this->quit(); });
        add_action(quit);

        auto menuBar  = Gio::Menu::create();
        auto quitItem = Gio::MenuItem::create("Quit", "app.quit");
        menuBar->append_item(quitItem);

        set_menubar(menuBar);
    }
};

int main(int argc, char* argv[]) {

#ifdef GL_LINUX_DEBUG
    //setenv("MESA_DEBUG", "flush,context,incomplete_tex,incomplete_fbo", 1);
    //setenv("LIBGL_DEBUG", "verbose", 1);
    //setenv("RADV_DEBUG", "img,info,shaders", 1);
    //setenv("AMD_DEBUG", "info", 1);
    //setenv("ZINK_DEBUG", "optimal_keys,validation,sync,mem", 1);
    setenv("mesa_glthread", "true", 1);
    setenv("GST_DEBUG", "2", 1);
    // setenv("GDK_BACKEND", "x11", 1);
#endif

    try {
        appInstance = std::make_shared<mainApp>();
    } catch (const std::exception& e) {
        SimpleGFX::exception::printException(e);
        return 100;
    }

    return appInstance->launch(argc, argv);
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
