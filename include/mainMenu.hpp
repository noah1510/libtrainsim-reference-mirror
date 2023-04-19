#pragma once

#include "simulator.hpp"

class mainMenu:public Gtk::Window, public SimpleGFX::eventHandle{
  private:
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;
    size_t selectedTrackID;
    size_t lastTrackID;
    size_t stopBegin;
    size_t stopEnd;
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

    void on_show() override;
};
