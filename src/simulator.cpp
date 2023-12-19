#include "simulator.hpp"

using namespace libtrainsim::core;
using namespace libtrainsim::Video;

using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::literals;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;

/*configSelectionWindow::configSelectionWindow():window{"configFileSelection"}{
    try{
        std::filesystem::path texpath = "application-json.png";
        fileTex = std::make_shared<texture>(texpath);
    }catch(...){
        std::throw_with_nested(std::runtime_error("cannot load file texture"));
    }
    closableWindow = false;
    showWindow = true;
}

configSelectionWindow::configSelectionWindow(std::string initialLocation):configSelectionWindow(){
    try{
        conf = std::make_shared<libtrainsim::core::simulatorConfiguration>(initialLocation, true);
        close = true;
    }catch(const std::exception& e){
        SimpleGFX::exception::print_exception(e);
        conf = nullptr;
        close = false;
        return;
    }
}

std::shared_ptr<libtrainsim::core::simulatorConfiguration> configSelectionWindow::getConfig(){
    return conf;
}

bool configSelectionWindow::closeWindow() const{
    return close;
}

void configSelectionWindow::content(){
    if(ImGui::Button("press here to select a configuration file.")){
        static NFD::UniquePathN outPath = nullptr;
        auto res = NFD::OpenDialog(outPath);
        if(res == NFD_OKAY){
            try{
                conf = std::make_shared<libtrainsim::core::simulatorConfiguration>(outPath.get(), true);
                close = true;
            }catch(const std::exception& e){
                SimpleGFX::exception::print_exception(e);
                conf = nullptr;
                close = false;
                return;
            }
        }
    }
}

simulatorConfigMenu::simulatorConfigMenu ( simulator& disp ) : tabPage("simulator"), display{disp}{}

void simulatorConfigMenu::content() {
    auto lastSnowState = display.enableSnow;
    auto lastDim = display.backgroundDim;

    ImGui::Checkbox("enable snow effects", &display.enableSnow);
    ImGui::SliderInt("background dimming amount", &display.backgroundDim, 0, 254);

    if(display.enableSnow != lastSnowState){
        if(display.enableSnow){
            display.video->addTexture(imguiHandler::getDarkenTexture(display.backgroundDim));
            display.video->addTexture(display.snow->getOutputTexture());
        }else{
            display.video->removeTexture(imguiHandler::getDarkenTexture(display.backgroundDim)->getName());
            display.video->removeTexture(display.snow->getOutputTexture()->getName());
        }
    }

    if(lastDim != display.backgroundDim){
        if(display.enableSnow){
            try{
                display.video->removeTexture(imguiHandler::getDarkenTexture(lastDim)->getName());
                display.video->removeTexture(display.snow->getOutputTexture()->getName());

                display.video->addTexture(imguiHandler::getDarkenTexture(display.backgroundDim));
                display.video->addTexture(display.snow->getOutputTexture());
            }catch(...){}
        }
    }
}*/

simulator::~simulator() {
    end();

    // imguiHandler::removeSettingsTab("simulator");
}

simulator::simulator(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _settings,
                     std::shared_ptr<libtrainsim::control::input_handler>       _input,
                     std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          _mainApp)
    : settings{std::move(_settings)},
      input{std::move(_input)},
      mainApp{std::move(_mainApp)},
      track{settings->getCurrentTrack()} {

    // load the physics
    try {
        phy = std::make_unique<libtrainsim::physics>(settings->getCurrentTrack());
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error initializing physics"));
    }

    // create the video window
    try {
        simulatorGroup = Gtk::WindowGroup::create();

#ifdef SIMULATOR_USE_FFMPEG
        video = Gtk::make_managed<outputWindow_PictureLibav>(settings, mainApp);
        SimpleGFX::onEventSlot outputWindowSlot = sigc::mem_fun(*video, &outputWindow_PictureLibav::onEvent);
#else
        video = Gtk::make_managed<outputWindow_PictureGstreamer>(settings, mainApp);
        SimpleGFX::onEventSlot outputWindowSlot = sigc::mem_fun(*video, &outputWindow_PictureGstreamer::onEvent);
#endif
        simulatorGroup->add_window(*video);

        settings->getInputManager()->registerHandler(outputWindowSlot);
        input->getKeyboardPoller()->addWindow(video);

        video->signal_close_request().connect(
            [this]() {
                end();
                return false;
            },
            true);
        // if(enableSnow){
        //     video->addTexture(imguiHandler::getDarkenTexture(backgroundDim));
        // }
    } catch (const std::exception& e) {
        std::throw_with_nested(std::runtime_error("Could not create simulator window"));
    }


    // load the status display
    /*
    try{
        statusWindow = Gtk::make_managed<libtrainsim::extras::statusDisplay>();
        simulatorGroup->add_window(*statusWindow);

        statusWindow->registerWithEventManager(settings->getInputManager().get(), 0);
        input->getKeyboardPoller()->addWindow(statusWindow);

        statusWindow->changeBeginPosition(track.firstLocation());
        statusWindow->changeEndPosition(track.lastLocation());
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create status window"));
    }
    */

    // display all windows in the group
    for (auto win : simulatorGroup->list_windows()) {
        mainApp->add_window(*win);
        win->set_visible(true);
    }


    /*
    //create the snow fx layer
    try{
        snow = std::make_unique<libtrainsim::extras::snowFx>(settings);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not load snowFx"));
    }
    */

    // load the input system
    try {
        input->startSimulation();
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Cannot create input_handler"));
    }

    video->present();

    hasError = false;

    // use the glib timer stuff to update the physics
    // video decoding is done always done in an async thread by the videoManager
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &simulator::updatePhysics), 3);

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &simulator::update), 16);

    // add the settings page after everything else has been fully added
    // imguiHandler::addSettingsTab(std::make_shared<simulatorConfigMenu>(*this));
}

bool simulator::updatePhysics() {
    if (hasError) {
        return false;
    };

    phy->setSpeedlevel(input->getSpeedAxis());

    if (input->emergencyFlag()) {
        phy->emergencyBreak();
    }

    phy->tick();
    auto loc = phy->getLocation();

    // check if the simulator has to be closed
    if (video->getRenderer().getDecoder().reachedEndOfFile() || phy->reachedEnd()) {
        mainApp->callDeffered(sigc::mem_fun(*this, &simulator::end));
        return false;
    }

    // get the next frame that will be displayed
    auto frame_num = track.data().getFrame(loc);

    // if there is already a frame that is being rendered
    // then this call will buffer the furthest frame to be rendered
    video->gotoFrame(frame_num);

    return true;
}

bool simulator::update() {
    if (hasError) {
        return false;
    };
    // actually render all of the windows
    // try{
    // if(enableSnow){
    // snow->updateTexture();
    //}
    //}catch(...){
    //    std::throw_with_nested(std::runtime_error("Error rendering the windows"));
    //}

    // display statistics (speed, location, frametime, etc.)
    // auto next_time = SimpleGFX::chrono::now();
    // statusWindow->appendFrametime(unit_cast(next_time-last_time));

    auto renderTimes = video->getNewRendertimes();
    // if(renderTimes.has_value()){
    //     for(auto time:renderTimes.value()){
    //         statusWindow->appendRendertime(time);
    //     }
    // }

    // statusWindow->changePosition(phy->getLocation());

    // statusWindow->setAcceleration(phy->getAcceleration());
    // statusWindow->setSpeedLevel(input->getSpeedAxis());

    // auto vel = phy->getVelocity();
    // snow->updateTrainSpeed(vel);
    // statusWindow->setVelocity(vel);

    // statusWindow->redrawGraphs();

    // if(input->closingFlag()){
    //     std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
    //     video->close();
    // }

    return true;
}

void simulator::end() {
    auto coreLogger = settings->getLogger();
    *coreLogger << SimpleGFX::loggingLevel::debug << "Closing simulator";

    if (hasError) {
        return;
    };
    hasError = true;
    settings->getInputManager()->raiseEvent(simulatorStopEvent::create());

    if (video->is_visible()) {
        video->close();
    }

    input->resetFlags();

    *coreLogger << SimpleGFX::loggingLevel::debug << "destroying physics";
    phy.reset();

    // std::cout << "   destroying snowfx" << std::endl;
    // snow.reset();

    *coreLogger << SimpleGFX::loggingLevel::normal << "simulator has exited";
}

bool simulator::hasErrored() {
    return hasError;
}
