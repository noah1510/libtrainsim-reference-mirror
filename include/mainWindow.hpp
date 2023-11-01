#pragma once

#include "simulator.hpp"
#include "trackSelectionWidget.hpp"

class mainWindow :public Gtk::ApplicationWindow, public SimpleGFX::eventHandle{
  private:
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;

    std::shared_ptr<libtrainsim::control::input_handler> input;

    std::unique_ptr<trackSelectionWidget> trackSelection = nullptr;
    std::unique_ptr<simulator> sim = nullptr;
  public:
    mainWindow(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf, const Glib::RefPtr<Gtk::Application>& application);
    ~mainWindow();

    bool onEvent(const SimpleGFX::inputEvent& event) override;
    bool on_close_request() override;
};
