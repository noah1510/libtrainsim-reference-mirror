#include "trackSelectionWidget.hpp"

#include <utility>

using namespace libtrainsim::core;
using namespace libtrainsim::Video;

using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::literals;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;

trackSelectionWidget::trackSelectionWidget(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf, const Glib::RefPtr<Gtk::Application>& application)
    : Gtk::Frame{},
      SimpleGFX::eventHandle(),
      conf{std::move(_conf)},
      app{application} {

    *conf->getLogger() << SimpleGFX::loggingLevel::debug << "Creating the track selection menu";

    reCreateTrackList();

    *conf->getLogger() << SimpleGFX::loggingLevel::normal << "track selection menu created";
}

void trackSelectionWidget::reCreateTrackList() {
    auto mainPane = Gtk::make_managed<Gtk::Paned>(Gtk::Orientation::HORIZONTAL);
    set_child(*mainPane);

    selectedTrackID = static_cast<int>(conf->getCurrentTrackID());
    lastTrackID     = selectedTrackID;
    stopBegin       = 0;
    stopEnd         = conf->getTrack(selectedTrackID).getStations().size() - 1;

    auto trackStack  = Gtk::make_managed<Gtk::Stack>();
    auto mainSidebar = Gtk::make_managed<Gtk::StackSidebar>();
    mainSidebar->set_stack(*trackStack);
    mainPane->set_resize_start_child(false);
    mainPane->set_start_child(*mainSidebar);
    mainPane->set_end_child(*trackStack);

    auto trackCount = conf->getTrackCount();
    for (uint64_t i = 0; i < trackCount; i++) {
        const auto& track = conf->getTrack(i);

        const auto& stations = track.getStations();

        // create the launch button for this track
        auto startButton = Gtk::make_managed<Gtk::Button>("Start Simulator");
        startButton->signal_clicked().connect([this, i]() {
            *conf->getLogger() << SimpleGFX::loggingLevel::normal << "Start button pressed";
            try {
                app->mark_busy();
                *conf->getLogger() << SimpleGFX::loggingLevel::debug << "Waiting for track to load";
                finishTrackLoad();
                app->unmark_busy();

                conf->getInputManager()->raiseEvent(simulatorStartEvent::create(i, stopBegin, stopEnd));
            } catch (const std::exception& e) {
                Helper::printException(e);
                app->quit();
                return;
            }
        });

        // show launch button below the selection
        auto pagePane = Gtk::make_managed<Gtk::Paned>(Gtk::Orientation::VERTICAL);
        pagePane->set_resize_end_child(false);
        pagePane->set_end_child(*startButton);

        // create pane with two lists for one for start and one for end station
        auto buttonPane = Gtk::make_managed<Gtk::Paned>(Gtk::Orientation::HORIZONTAL);
        pagePane->set_start_child(*buttonPane);

        auto              buttonListBegin = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
        auto              buttonListEnd   = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
        Gtk::CheckButton* button0Begin;
        Gtk::CheckButton* button0End;

        // add all begin station buttons
        for (uint64_t j = 0; j < stations.size() - 1; j++) {
            auto* btn = Gtk::make_managed<Gtk::CheckButton>(stations[j].name());

            // radioButtons.emplace_back(btn);
            buttonListBegin->append(*btn);
            if (j) {
                btn->set_group(*button0Begin);
            } else {
                button0Begin = btn;
                btn->set_active();
            }
            btn->signal_toggled().connect([this, i, j, btn]() {
                if (btn->get_active()) {
                    selectedTrackID = i;
                    stopBegin       = j;
                    asyncTrackLoads.emplace_back(std::async(std::launch::async, [this, i]() { conf->ensureTrack(i); }));
                }
            });
        }

        // add all end station buttons
        for (uint64_t j = 1; j < stations.size(); j++) {
            auto* btn = Gtk::make_managed<Gtk::CheckButton>(stations[j].name());
            if (j == stations.size() - 1) {
                btn->set_active();
            }

            // radioButtons.emplace_back(btn);
            buttonListEnd->append(*btn);
            if (j > 1) {
                btn->set_group(*button0End);
            } else {
                button0End = btn;
            }
            btn->signal_toggled().connect([this, i, j, btn]() {
                if (btn->get_active()) {
                    selectedTrackID = i;
                    stopEnd         = j;
                    asyncTrackLoads.emplace_back(std::async(std::launch::async, [this, i]() { conf->ensureTrack(i); }));
                }
            });
        }

        // add all buttons to the pane and show it
        buttonPane->set_start_child(*buttonListBegin);
        buttonPane->set_end_child(*buttonListEnd);

        trackStack->add(*pagePane, track.getName(), track.getName());
    }
}

trackSelectionWidget::~trackSelectionWidget() {
    finishTrackLoad();
}

void trackSelectionWidget::on_show() {
    *conf->getLogger() << SimpleGFX::loggingLevel::debug << "Showing main menu";
    Gtk::Frame::on_show();
    // reCreateTrackList();
}

void trackSelectionWidget::finishTrackLoad() {
    // wait for all tracks to finish loading before starting the track
    for (auto& task : asyncTrackLoads) {
        if (task.valid()) {
            task.wait();
            task.get();
        }
    }
    asyncTrackLoads.clear();
}

