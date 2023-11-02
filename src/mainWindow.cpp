#include "mainWindow.hpp"

using namespace libtrainsim::core;
using namespace libtrainsim::Video;

using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::literals;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;

mainWindow::mainWindow(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf, const Glib::RefPtr<Gtk::Application>& application)
    : Gtk::ApplicationWindow{application},
      SimpleGFX::eventHandle(),
      conf{std::move(_conf)} {

    *conf->getLogger() << SimpleGFX::loggingLevel::debug << "Creating the main menu";

    input = std::make_shared<libtrainsim::control::input_handler>(conf);
    input->registerWithEventManager(conf->getInputManager().get(), 0);

    sim.reset();
    sim = nullptr;

    set_title("Main Menu");
    set_default_size(1280, 720);
    //set_show_menubar(true);

    //create the track selection widget
    trackSelection = std::make_unique<trackSelectionWidget>(conf, application);
    set_child(*trackSelection);

    *conf->getLogger() << SimpleGFX::loggingLevel::normal << "Main menu created";
}

mainWindow::~mainWindow() {}

bool mainWindow::onEvent(const SimpleGFX::inputEvent& event) {
    // try to parse the event into a simulatorStartEvent
    auto startEvent = simulatorStartEvent::parse(event);
    if (startEvent.has_value()) {
        // if the event is a simulatorStartEvent, start the simulator
        // and return true to indicate that the event was handled
        auto selectedTrackID = startEvent.value().trackIndex;
        auto stopBegin       = startEvent.value().stopBeginIndex;
        auto stopEnd         = startEvent.value().stopEndIndex;

        //set the correct track and stop in the configuration
        conf->selectTrack(selectedTrackID);
        const auto& stops = conf->getCurrentTrack().getStations();
        conf->getTrack(selectedTrackID).setLastLocation(stops[stopEnd].position());

        //print some debug info about the selected track
        *conf->getLogger() << SimpleGFX::loggingLevel::detail << "Setting start location: " << stopBegin << " to "
                           << conf->getCurrentTrack().getStations()[stopBegin].name();
        conf->getTrack(selectedTrackID).setFirstLocation(stops[stopBegin].position());
        *conf->getLogger() << SimpleGFX::loggingLevel::detail << "Setting end location: " << stopEnd << " to "
                           << conf->getCurrentTrack().getStations()[stopEnd].name();

        //create the simulator and start it
        *conf->getLogger() << SimpleGFX::loggingLevel::debug << "Creating the simulator";
        sim = std::make_unique<simulator>(conf, input, get_application());


        //hide the track selection since we now have a simulator
        trackSelection->hide();

        return true;
    }

    //try to parse the event into a simulatorStopEvent
    auto stopEvent = simulatorStopEvent::parse(event);
    if (stopEvent.has_value()) {
        // if the event is a simulatorStopEvent, stop the simulator
        // and return true to indicate that the event was handled
        *conf->getLogger() << SimpleGFX::loggingLevel::normal << "Stopping the simulator";

        sim.reset();
        sim = nullptr;

        trackSelection->show();

        return true;
    }

    // if the event is not a simulatorStartEvent, return false
    // since this event is not handled by this class
    return false;
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
