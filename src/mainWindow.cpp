#include "mainWindow.hpp"

using namespace libtrainsim::core;
using namespace libtrainsim::Video;

using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::literals;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;

mainWindow::mainWindow(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf,
                       const std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>&   application)
    : Gtk::ApplicationWindow{application},
      conf{std::move(_conf)},
      mainAppLauncher{application} {

    *conf->getLogger() << SimpleGFX::loggingLevel::debug << "Creating the main menu";

    input = std::make_shared<libtrainsim::control::input_handler>(conf);
    conf->getInputManager()->registerHandler(*input);
    conf->getInputManager()->registerHandler(*this);

    sim.reset();
    sim = nullptr;

    set_title("Main Menu");
    set_default_size(1280, 720);
    // set_show_menubar(true);

    // create the track selection widget
    trackSelection = std::make_unique<trackSelectionWidget>(conf, mainAppLauncher);
    set_child(*trackSelection);

    *conf->getLogger() << SimpleGFX::loggingLevel::normal << "Main menu created";
}

mainWindow::~mainWindow() {
    sec_untrack();
}

void mainWindow::operator()(const SimpleGFX::inputEvent& event, bool& handled) {
    // try to parse the event into a simulatorStartEvent
    auto startEvent = simulatorStartEvent::parse(event);
    if (startEvent.has_value()) {
        // if the event is a simulatorStartEvent, start the simulator
        // and return true to indicate that the event was handled
        auto selectedTrackID = startEvent.value().trackIndex;
        auto stopBegin       = startEvent.value().stopBeginIndex;
        auto stopEnd         = startEvent.value().stopEndIndex;

        // set the correct track and stop in the configuration
        conf->selectTrack(selectedTrackID);
        const auto& stops = conf->getCurrentTrack().getStations();
        conf->getTrack(selectedTrackID).setLastLocation(stops[stopEnd].position());

        // print some debug info about the selected track
        *conf->getLogger() << SimpleGFX::loggingLevel::detail << "Setting start location: " << stopBegin << " to "
                           << conf->getCurrentTrack().getStations()[stopBegin].name();
        conf->getTrack(selectedTrackID).setFirstLocation(stops[stopBegin].position());
        *conf->getLogger() << SimpleGFX::loggingLevel::detail << "Setting end location: " << stopEnd << " to "
                           << conf->getCurrentTrack().getStations()[stopEnd].name();

        // create the simulator and start it
        // that call has to be deffered since this function is not called on the main thread
        mainAppLauncher->callDeffered([this]() {
            if (sim != nullptr) {
                return;
            }
            *conf->getLogger() << SimpleGFX::loggingLevel::debug << "Creating the simulator";
            sim = std::make_unique<simulator<OUTPUT_WINDOW_CLASS>>(conf, input, mainAppLauncher);
            // trackSelection->hide();
        });

        handled = true;
        return;
    }

    // try to parse the event into a simulatorStopEvent
    auto stopEvent = simulatorStopEvent::parse(event);
    if (stopEvent.has_value()) {
        mainAppLauncher->callDeffered([this]() {
            if (sim == nullptr) {
                return;
            }
            // if the event is a simulatorStopEvent, stop the simulator
            // and return true to indicate that the event was handled
            *conf->getLogger() << SimpleGFX::loggingLevel::normal << "Stopping the simulator";

            sim.reset();
            sim = nullptr;

            // trackSelection->show();
        });

        handled = true;
        return;
    }

    // if the event is not a simulatorStartEvent, return false
    // since this event is not handled by this class
    return;
}

bool mainWindow::on_close_request() {
    *conf->getLogger() << SimpleGFX::loggingLevel::normal << "closing main menu";

    if (sim != nullptr) {
        sim.reset();
        sim = nullptr;
    }

    trackSelection.reset();
    trackSelection = nullptr;
    unset_child();

    return Gtk::Window::on_close_request();
}
