#pragma once

#include "simulator.hpp"
#include "trackSelectionWidget.hpp"

#define OUTPUT_WINDOW_CLASS libtrainsim::Video::outputWindow_PictureLibav

class mainWindow :public Gtk::ApplicationWindow, public SimpleGFX::tracked_eventHandle{
  private:
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;

    std::shared_ptr<libtrainsim::control::input_handler> input;

    std::unique_ptr<trackSelectionWidget> trackSelection = nullptr;
    std::unique_ptr<simulator<OUTPUT_WINDOW_CLASS>> sim = nullptr;

    std::shared_ptr<SimpleGFX::SimpleGL::appLauncher> mainAppLauncher;
  public:
    mainWindow(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf, const std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>& application);
    ~mainWindow();

    void operator()(const SimpleGFX::inputEvent& event, bool& handled) override;
    bool on_close_request() override;
};
