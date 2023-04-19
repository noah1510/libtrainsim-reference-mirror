#include "simulator.hpp"
#include "mainMenu.hpp"

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
        libtrainsim::core::Helper::print_exception(e);
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
                libtrainsim::core::Helper::print_exception(e);
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

simulator::~simulator(){
    end();
    
    //imguiHandler::removeSettingsTab("simulator");
    
}

simulator::simulator(
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> _settings,
    std::shared_ptr<libtrainsim::control::input_handler> _input,
    Glib::RefPtr<Gtk::Application> _mainApp,
    mainMenu& _mainMenu
):
    settings{_settings},
    input{_input},
    mainApp{_mainApp},
    track{settings->getCurrentTrack()},
    mmainMenu{_mainMenu}
{
    
    //load the physics
    try{
        phy = std::make_unique<libtrainsim::physics>(settings->getCurrentTrack());
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error initializing physics"));
    }
    
    //create the video window
    try{
        simulatorGroup = Gtk::WindowGroup::create();

        video = Gtk::make_managed<videoManager>(settings);
        simulatorGroup->add_window(*video);

        video->registerWithEventManager(settings->getInputManager().get());
        input->getKeyboardPoller()->addWindow(video);

        video->signal_close_request().connect([this](){
            end();
            return false;
        },false);
        //if(enableSnow){
        //    video->addTexture(imguiHandler::getDarkenTexture(backgroundDim));
        //}
    }catch(const std::exception& e){
        std::throw_with_nested(std::runtime_error("Could not create simulator window"));
    }


    //load the status display
    try{
        statusWindow = Gtk::make_managed<libtrainsim::extras::statusDisplay>();
        simulatorGroup->add_window(*statusWindow);

        statusWindow->registerWithEventManager(settings->getInputManager().get());
        input->getKeyboardPoller()->addWindow(statusWindow);

        statusWindow->changeBeginPosition(track.firstLocation());
        statusWindow->changeEndPosition(track.lastLocation());
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create status window"));
    }


    //display all windows in the group
    for(auto win:simulatorGroup->list_windows()){
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
    
    //load the input system
    try{
        input->startSimulation();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Cannot create input_handler"));
    }

    hasError = false;
    
    //start the physics and video decode updates in a separate thread
    physicsLoop = std::async(std::launch::async, [this](){
        auto last_time = libtrainsim::core::Helper::now();

        do{
            updatePhysics();

            if(libtrainsim::core::Helper::now() - last_time < 5ms){
                std::this_thread::sleep_until(last_time + 5ms);
            }
            last_time = libtrainsim::core::Helper::now();
        }while(!hasErrored());

    });

    updateLoop = std::async(std::launch::async, [this](){do{update();}while(!hasErrored());});
    
    //add the settings page after everything else has been fully added
    //imguiHandler::addSettingsTab(std::make_shared<simulatorConfigMenu>(*this));
}

void simulator::updatePhysics(){

    phy->setSpeedlevel(input->getSpeedAxis());

    if(input->emergencyFlag()){
        phy->emergencyBreak();
    }

    phy->tick();
    auto loc = phy->getLocation();

    //check if the simulator has to be closed
    if(video->getDecoder().reachedEndOfFile() || phy->reachedEnd()){
        end();
        return;
    }

    //get the next frame that will be displayed
    auto frame_num = track.data().getFrame(loc);

    //if there is already a frame that is being rendered
    //then this call will buffer the furthest frame to be rendered
    video->gotoFrame(frame_num);
}

void simulator::update(){
    static auto last_time = libtrainsim::core::Helper::now();
    //actually render all of the windows
    try{
        if(enableSnow){
            //snow->updateTexture();
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error rendering the windows"));
    }

    //display statistics (speed, location, frametime, etc.)
    auto next_time = libtrainsim::core::Helper::now();

    time_si frametime = unit_cast(next_time-last_time, multiplier(std::milli::type{}));
    statusWindow->appendFrametime(frametime);

    auto renderTimes = video->getNewRendertimes();
    if(renderTimes.has_value()){
        for(auto time:renderTimes.value()){
            statusWindow->appendRendertime(time);
        }
    }

    statusWindow->changePosition(phy->getLocation());

    statusWindow->setAcceleration(phy->getAcceleration());
    statusWindow->setSpeedLevel(input->getSpeedAxis());

    auto vel = phy->getVelocity();
    //snow->updateTrainSpeed(vel);
    statusWindow->setVelocity(vel);

    statusWindow->redrawGraphs();

    while(Helper::now()-last_time < 8ms){
        std::this_thread::sleep_until(last_time+8ms);
    }

    last_time = Helper::now();

    //if(input->closingFlag()){
    //    std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
    //    video->close();
    //}
}

void simulator::end(){
    auto coreLogger = settings->getLogger();
    coreLogger->logMessage("Closing simulator", SimpleGFX::loggingLevel::detail);

    errorMutex.lock();
    if(hasError){return;};
    hasError = true;
    errorMutex.unlock();

    mainApp->mark_busy();

    if(video->is_visible()){
        video->close();
    }

    mainApp->add_window(mmainMenu);
    mmainMenu.set_visible(true);

    coreLogger->logMessage("waiting for threads to end", SimpleGFX::loggingLevel::detail);
    if(physicsLoop.valid()){
        physicsLoop.wait();
        //physicsLoop.get();
    }

    coreLogger->logMessage("waiting for update thread to end", SimpleGFX::loggingLevel::detail);
    if(updateLoop.valid()){
        updateLoop.wait();
        //updateLoop.get();
    }

    std::scoped_lock lock{errorMutex};
    input->resetFlags();

    coreLogger->logMessage("destroying physics", SimpleGFX::loggingLevel::detail);
    phy.reset();

    //std::cout << "   destroying snowfx" << std::endl;
    //snow.reset();

    coreLogger->logMessage("resetting the simulator group", SimpleGFX::loggingLevel::detail);
    simulatorGroup.reset();

    coreLogger->logMessage("simulator has exited", SimpleGFX::loggingLevel::detail);
    mainApp->unmark_busy();
}

bool simulator::hasErrored(){
    std::shared_lock lock{errorMutex};
    return hasError;
}
