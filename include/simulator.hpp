#pragma once

//#include "snowFx.hpp"

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

//class simulatorConfigMenu;

class mainWindow;

class simulator{
    //friend class simulatorConfigMenu;
    private:
        std::shared_ptr<libtrainsim::core::simulatorConfiguration> settings;
        std::shared_ptr<libtrainsim::control::input_handler> input;
        Glib::RefPtr<Gtk::Application> mainApp;
        const libtrainsim::core::Track& track;
        Glib::RefPtr<Gtk::WindowGroup> simulatorGroup;

        std::atomic<bool> hasError = true;

        std::unique_ptr<libtrainsim::physics> phy;

        libtrainsim::Video::videoManager* video;
        libtrainsim::extras::statusDisplay* statusWindow;
        //std::unique_ptr<libtrainsim::extras::snowFx> snow;

        bool enableSnow = false;
        int backgroundDim = 20;

    public:
        simulator(
            std::shared_ptr<libtrainsim::core::simulatorConfiguration> _settings,
            std::shared_ptr<libtrainsim::control::input_handler> _input,
            Glib::RefPtr<Gtk::Application> _mainApp
        );
        ~simulator();
        bool hasErrored();
        void end();

        bool updatePhysics();
        bool update();
};

/*class simulatorConfigMenu : public SimpleGFX::SimpleGL::tabPage {
  private:
    void content() override;
    simulator& display;
  public:
    simulatorConfigMenu(simulator& disp);
};*/
