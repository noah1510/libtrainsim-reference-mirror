#include "mainMenu.hpp"

using namespace libtrainsim::core;
using namespace libtrainsim::Video;

using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::literals;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;

mainMenu::mainMenu(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf) : Gtk::Window{}, SimpleGFX::eventHandle(), conf{_conf}{

    conf->getLogger()->logMessage("Creating the main menu", SimpleGFX::loggingLevel::detail);

    input = std::make_shared<libtrainsim::control::input_handler>(conf);
    input->registerWithEventManager(conf->getInputManager().get());

    sim.reset();
    sim = nullptr;

    set_title("Main Menu");
    set_default_size(1280, 720);

    reCreateTrackList();

    conf->getLogger()->logMessage("Main menu created", SimpleGFX::loggingLevel::normal);
}

void mainMenu::reCreateTrackList() {
    auto mainPane = Gtk::make_managed<Gtk::Paned>(Gtk::Orientation::HORIZONTAL);
    set_child(*mainPane);

    selectedTrackID = static_cast<int>(conf->getCurrentTrackID());
    lastTrackID = selectedTrackID;
    stopBegin = 0;
    stopEnd = conf->getTrack(selectedTrackID).getStations().size() - 1;

    auto trackStack = Gtk::make_managed<Gtk::Stack>();
    auto mainSidebar = Gtk::make_managed<Gtk::StackSidebar>();
    mainSidebar->set_stack(*trackStack);
    mainPane->set_resize_start_child(false);
    mainPane->set_start_child(*mainSidebar);
    mainPane->set_end_child(*trackStack);

    auto trackCount = conf->getTrackCount();
    for(uint64_t i = 0; i < trackCount;i++){
        const auto& track = conf->getTrack(i);

        const auto& stations = track.getStations();

        //create the lauch button for this track
        auto startButton = Gtk::make_managed<Gtk::Button>("Start Simulator");
        startButton->signal_clicked().connect([this, i](){
            conf->getLogger()->logMessage("Start button pressed", SimpleGFX::loggingLevel::normal);
            try{
                get_application()->mark_busy();

                conf->getLogger()->logMessage("Waiting for track to load", SimpleGFX::loggingLevel::detail);
                for(auto j = asyncTrackLoads.begin(); j < asyncTrackLoads.end(); j++){
                    if(j->valid()){
                        j->wait();
                        j = asyncTrackLoads.erase(j);
                    }
                }

                conf->getLogger()->logMessage("selecting track: " + conf->getTrack(i).getName(), SimpleGFX::loggingLevel::detail);
                conf->selectTrack(i);

                conf->getLogger()->logMessage("Setting start and end location", SimpleGFX::loggingLevel::detail);
                const auto& stops = conf->getCurrentTrack().getStations();
                conf->getTrack(selectedTrackID).setFirstLocation(stops[stopBegin].position());
                conf->getTrack(selectedTrackID).setLastLocation(stops[stopEnd].position());

                conf->getLogger()->logMessage("Creating the simulator", SimpleGFX::loggingLevel::detail);
                sim = std::make_unique<simulator>(conf, input, get_application(), *this);

                get_application()->unmark_busy();
            }catch(const std::exception& e){
                Helper::printException(e);
                close();
                return;
            }
            hide();
        });

        //show launch button below the selection
        auto pagePane =  Gtk::make_managed<Gtk::Paned>(Gtk::Orientation::VERTICAL);
        pagePane->set_resize_end_child(false);
        pagePane->set_end_child(*startButton);

        //create pane with two lists for one for start and one for end station
        auto buttonPane = Gtk::make_managed<Gtk::Paned>(Gtk::Orientation::HORIZONTAL);
        pagePane->set_start_child(*buttonPane);

        auto buttonListBegin = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
        auto buttonListEnd = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
        Gtk::CheckButton* button0Begin;
        Gtk::CheckButton* button0End;

        //add all begin station buttons
        for(uint64_t j = 0; j < stations.size() - 1;j++){
            auto* btn = Gtk::make_managed<Gtk::CheckButton>(stations[j].name());

            //radioButtons.emplace_back(btn);
            buttonListBegin->append(*btn);
            if(j){
                btn->set_group(*button0Begin);
            }else{
                button0Begin = btn;
                btn->set_active();
            }
            btn->signal_toggled().connect( [this, i, j, btn](){
                if(btn->get_active()){
                    selectedTrackID = i;
                    stopBegin = j;
                    asyncTrackLoads.emplace_back(std::async(std::launch::async,[this,i](){conf->ensureTrack(i);}));
                }
            } );
        }

        //add all end station buttons
        for(uint64_t j = 1; j < stations.size();j++){
            auto* btn = Gtk::make_managed<Gtk::CheckButton>(stations[j].name());
            btn->set_active();

            //radioButtons.emplace_back(btn);
            buttonListEnd->append(*btn);
            if(j > 1){
                btn->set_group(*button0End);
            }else{
                button0End = btn;
            }
            btn->signal_toggled().connect( [this, i, j, btn](){
                if(btn->get_active()){
                    selectedTrackID = i;
                    stopEnd = j;
                    asyncTrackLoads.emplace_back(std::async(std::launch::async,[this,i](){conf->ensureTrack(i);}));
                }
            } );
        }

        //add all buttons to the pane and show it
        buttonPane->set_start_child(*buttonListBegin);
        buttonPane->set_end_child(*buttonListEnd);

        trackStack->add(*pagePane, track.getName(), track.getName());

    }

}

mainMenu::~mainMenu(){
    finishTrackLoad();
}

void mainMenu::on_show(){
    return Gtk::Window::on_show();
}

void mainMenu::finishTrackLoad() {
    //wait for all tracks to finish loading before starting the track
    for(auto& task:asyncTrackLoads){
        if(task.valid()){
            task.wait();
            task.get();
        }
    }
    asyncTrackLoads.clear();
}

bool mainMenu::shouldStart() const {
    return ShouldStart;
}

int mainMenu::getSelectedTrack() const {
    return static_cast<int>(selectedTrackID);
}

std::pair<int, int> mainMenu::getStopIDs() const {
    return {stopBegin, stopEnd};
}

