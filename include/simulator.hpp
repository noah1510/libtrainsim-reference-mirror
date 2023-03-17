#pragma once

#include "simulator_config.hpp"
#include "video.hpp"
#include "physics.hpp"
#include "control.hpp"

//#include "statusDisplay.hpp"
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

class simulator;
class mainMenu:public Gtk::Window{
  private:
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;
    int selectedTrackID;
    int lastTrackID;
    int stopBegin;
    int stopEnd;
    bool ShouldStart = false;
    void reCreateTrackList();
    std::vector<std::future<void>> asyncTrackLoads;

    std::shared_ptr<libtrainsim::control::input_handler> input;
    std::unique_ptr<simulator> sim;
  public:
    mainMenu(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf);
    ~mainMenu();
    
    bool shouldStart() const;
    void finishTrackLoad();
    int getSelectedTrack() const;
    std::pair<int, int> getStopIDs() const;
};

class simulator{
    //friend class simulatorConfigMenu;
    private:
        std::shared_ptr<libtrainsim::core::simulatorConfiguration> settings;
        std::shared_ptr<libtrainsim::control::input_handler> input;
        Glib::RefPtr<Gtk::Application> mainApp;
        const libtrainsim::core::Track& track;
        mainMenu& mmainMenu;
        Glib::RefPtr<Gtk::WindowGroup> simulatorGroup;

        bool hasError = true;
        std::shared_mutex errorMutex;

        std::future<void> physicsLoop;
        std::future<void> updateLoop;
        std::vector<uint64_t> callbackIDs;

        std::unique_ptr<libtrainsim::physics> phy;

        libtrainsim::Video::videoManager* video;
        //std::unique_ptr<libtrainsim::extras::statusDisplay> statusWindow;
        //std::unique_ptr<libtrainsim::extras::snowFx> snow;


        bool enableSnow = false;
        int backgroundDim = 20;

    public:
        simulator(
            std::shared_ptr<libtrainsim::core::simulatorConfiguration> _settings,
            std::shared_ptr<libtrainsim::control::input_handler> _input,
            Glib::RefPtr<Gtk::Application> _mainApp,
            mainMenu& _mainMenu
        );
        ~simulator();
        bool hasErrored();
        void end();

        void updatePhysics();
        void update();
};

/*class simulatorConfigMenu : public SimpleGFX::SimpleGL::tabPage {
  private:
    void content() override;
    simulator& display;
  public:
    simulatorConfigMenu(simulator& disp);
};*/
