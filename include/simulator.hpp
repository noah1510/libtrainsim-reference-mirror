#pragma once

#include "simulator_config.hpp"
#include "video.hpp"
#include "physics.hpp"
#include "control.hpp"

#include "statusDisplay.hpp"
#include "snowFx.hpp"

class simulatorConfigMenu;

class mainMenu:public libtrainsim::Video::window{
    private:
      std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;
      int selectedTrackID;
      int lastTrackID;
      int stopBegin;
      int stopEnd;
      bool ShouldStart = false;
      std::vector<std::future<void>> asycTrackLoads;
      void content() override;
    public:
      mainMenu(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf);
      ~mainMenu();
      
      bool shouldStart() const;
      void finishTrackLoad();
      int getSelectedTrack() const;
      std::pair<int, int> getStopIDs() const;
};

class simulator{
    friend class simulatorConfigMenu;
    private:
        bool hasError = true;
        std::shared_mutex errorMutex;

        libtrainsim::core::input_axis Speedlevel;

        std::future<void> physicsLoop;

        const libtrainsim::core::Track& track;
        std::unique_ptr<libtrainsim::physics> phy;

        std::unique_ptr<libtrainsim::Video::videoManager> video;
        std::unique_ptr<libtrainsim::extras::statusDisplay> statusWindow;
        std::unique_ptr<libtrainsim::extras::snowFx> snow;
        std::unique_ptr<libtrainsim::control::input_handler> input;

        bool enableSnow = false;
        int backgroundDim = 20;

    public:
        simulator(std::shared_ptr<libtrainsim::core::simulatorConfiguration> settings);
        ~simulator();
        bool hasErrored();
        void end();
        
        void emergencyBreak();
        void update();

        void serial_speedlvl(libtrainsim::core::input_axis Slvl);
};

class simulatorConfigMenu : public libtrainsim::Video::tabPage {
  private:
    void content() override;
    simulator& display;
  public:
    simulatorConfigMenu(simulator& disp);
};
