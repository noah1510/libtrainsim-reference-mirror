#pragma once

// #include "snowFx.hpp"

/*class configSelectionWindow :public Gtk::Window{
  private:
    void content() override;
    bool close = false;
    std::shared_ptr<SimpleGFX::SimpleGL::texture> fileTex = nullptr;
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf = nullptr;
  public:
    configSelectionWindow();
    configSelectionWindow(std::string initialLocation);
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> getConfig();
    bool closeWindow() const;
};*/

// class simulatorConfigMenu;

#include "unit_system.hpp"
class mainWindow;

template<class OUTPUT_WINDOW_CLASS>
class simulator : public sec_sigc::sec_trackable {
    // friend class simulatorConfigMenu;
  private:
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> settings;
    std::shared_ptr<libtrainsim::control::input_handler>       input;
    std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          mainApp;
    const libtrainsim::core::Track&                            track;
    Glib::RefPtr<Gtk::WindowGroup>                             simulatorGroup;

    std::atomic<bool> hasError = true;

    std::unique_ptr<libtrainsim::physics> phy;

    OUTPUT_WINDOW_CLASS* video;

    libtrainsim::extras::statusDisplay*                                   statusOverlay;
    // std::unique_ptr<libtrainsim::extras::snowFx> snow;

    bool enableSnow    = false;
    int  backgroundDim = 20;

  public:
    simulator(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _settings,
              std::shared_ptr<libtrainsim::control::input_handler>       _input,
              std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          _mainApp): settings{std::move(_settings)},
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

            video = Gtk::make_managed<OUTPUT_WINDOW_CLASS>(settings, mainApp);
            simulatorGroup->add_window(*video);

            settings->getInputManager()->registerHandler(*video);
            input->getKeyboardPoller()->addWidget(*video);

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
       
        try{
            statusOverlay = Gtk::make_managed<libtrainsim::extras::statusDisplay>(mainApp);
            

            settings->getInputManager()->registerHandler(*statusOverlay);
            input->getKeyboardPoller()->addWidget(*statusOverlay);

            statusOverlay->changeBeginPosition(track.firstLocation());
            statusOverlay->changeEndPosition(track.lastLocation());
            
            video->getOverlayContainer().add_overlay(*statusOverlay);
        }catch(...){
            std::throw_with_nested(std::runtime_error("Could not create status window"));
        }
        

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
    ~simulator(){end();};
    bool hasErrored(){return hasError;}

    void end(){
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

    bool updatePhysics(){
        if (hasError) {
            return false;
        };

        phy->setSpeedlevel(input->getSpeedAxis());

        if (input->emergencyFlag(phy->emergencyBreaking())) {
            phy->emergencyBreak();
        }

        phy->tick();
        auto loc = phy->getLocation();

        // check if the simulator has to be closed
        if (video->getRenderer().getDecoder().reachedEndOfFile() || phy->reachedEnd()) {
            mainApp->callDeffered([this]() { this->end(); }, sec_getID());
            return false;
        }

        // get the next frame that will be displayed
        auto frame_num = track.getFrame(loc);

        // if there is already a frame that is being rendered
        // then this call will buffer the furthest frame to be rendered
        video->gotoFrame(frame_num);

        return true;
    }

    bool update(){
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
        static auto last_time = SimpleGFX::chrono::now();
        auto next_time = SimpleGFX::chrono::now();
        statusOverlay->appendFrametime(sakurajin::unit_system::unit_cast(next_time-last_time));
        last_time = next_time;

        auto renderTimes = video->getNewRendertimes();
        if(renderTimes.has_value()){
            for(auto time:renderTimes.value()){
                statusOverlay->appendRendertime(time);
            }
        }

        statusOverlay->changePosition(phy->getLocation());

        statusOverlay->setAcceleration(phy->getAcceleration());
        statusOverlay->setSpeedLevel(input->getSpeedAxis());

        auto vel = phy->getVelocity();
        // snow->updateTrainSpeed(vel);
        statusOverlay->setVelocity(vel);

        statusOverlay->redrawGraphs();

        // if(input->closingFlag()){
        //     std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
        //     video->close();
        // }

        return true;
    }
};

/*class simulatorConfigMenu : public SimpleGFX::SimpleGL::tabPage {
  private:
    void content() override;
    simulator& display;
  public:
    simulatorConfigMenu(simulator& disp);
};*/
