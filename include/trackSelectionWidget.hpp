#pragma once

#include "simulator_includes.hpp"

class trackSelectionWidget : public Gtk::Frame, public SimpleGFX::eventHandle {
  private:
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;
    std::vector<std::future<void>>                             asyncTrackLoads;
    Glib::RefPtr<Gtk::Application>                             app;

    size_t selectedTrackID = 0;
    size_t lastTrackID     = 0;
    size_t stopBegin       = 0;
    size_t stopEnd         = 0;

    void reCreateTrackList();

  public:
    trackSelectionWidget(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf,
                         const Glib::RefPtr<Gtk::Application>&                      application);

    ~trackSelectionWidget();

    void finishTrackLoad();

    void on_show() override;
};
