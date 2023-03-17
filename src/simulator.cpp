#include "simulator.hpp"

using namespace libtrainsim::core;
using namespace libtrainsim::Video;

using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::literals;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;

/*configSelectionWindow::configSelectionWindow():window{"configFileSelection"}{
    try{
        std::filesystem::path texpath = "application-json.png";
        fileTex = std::make_shared<texture>(texpath);
    }catch(...){
        std::throw_with_nested(std::runtime_error("cannot load file texture"));
    }
    closableWindow = false;
    showWindow = true;
}

configSelectionWindow::configSelectionWindow(std::string initialLocation):configSelectionWindow(){
    try{
        conf = std::make_shared<libtrainsim::core::simulatorConfiguration>(initialLocation, true);
        close = true;
    }catch(const std::exception& e){
        libtrainsim::core::Helper::print_exception(e);
        conf = nullptr;
        close = false;
        return;
    }
}

std::shared_ptr<libtrainsim::core::simulatorConfiguration> configSelectionWindow::getConfig(){
    return conf;
}

bool configSelectionWindow::closeWindow() const{
    return close;
}

void configSelectionWindow::content(){
    if(ImGui::Button("press here to select a configuration file.")){
        static NFD::UniquePathN outPath = nullptr;
        auto res = NFD::OpenDialog(outPath);
        if(res == NFD_OKAY){
            try{
                conf = std::make_shared<libtrainsim::core::simulatorConfiguration>(outPath.get(), true);
                close = true;
            }catch(const std::exception& e){
                libtrainsim::core::Helper::print_exception(e);
                conf = nullptr;
                close = false;
                return;
            }
        }
    }
}

simulatorConfigMenu::simulatorConfigMenu ( simulator& disp ) : tabPage("simulator"), display{disp}{}

void simulatorConfigMenu::content() {
    auto lastSnowState = display.enableSnow;
    auto lastDim = display.backgroundDim;
        
    ImGui::Checkbox("enable snow effects", &display.enableSnow);
    ImGui::SliderInt("background dimming amount", &display.backgroundDim, 0, 254);
    
    if(display.enableSnow != lastSnowState){
        if(display.enableSnow){
            display.video->addTexture(imguiHandler::getDarkenTexture(display.backgroundDim));
            display.video->addTexture(display.snow->getOutputTexture());
        }else{
            display.video->removeTexture(imguiHandler::getDarkenTexture(display.backgroundDim)->getName());
            display.video->removeTexture(display.snow->getOutputTexture()->getName());
        }
    }

    if(lastDim != display.backgroundDim){
        if(display.enableSnow){
            try{
                display.video->removeTexture(imguiHandler::getDarkenTexture(lastDim)->getName());
                display.video->removeTexture(display.snow->getOutputTexture()->getName());

                display.video->addTexture(imguiHandler::getDarkenTexture(display.backgroundDim));
                display.video->addTexture(display.snow->getOutputTexture());
            }catch(...){}
        }
    }
}*/

mainMenu::mainMenu(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf) : Gtk::Window{}, conf{_conf}{

    input = std::make_shared<libtrainsim::control::input_handler>(conf); Gtk::Window::on_realize();

    std::cout << "realizing the main Menu" << std::endl;
    sim.reset();
    sim = nullptr;

    set_title("Main Menu");
    set_default_size(1280, 720);

    reCreateTrackList();
}

void mainMenu::reCreateTrackList() {
    auto mainPane = Gtk::make_managed<Gtk::Paned>(Gtk::Orientation::HORIZONTAL);
    //auto mainPane = Gtk::make_managed<Gtk::Box>();
    //mainPane->set_homogeneous(false);
    set_child(*mainPane);

    auto trackStack = Gtk::make_managed<Gtk::Stack>();
    auto mainSidebar = Gtk::make_managed<Gtk::StackSidebar>();
    mainSidebar->set_stack(*trackStack);
    mainPane->set_resize_start_child(false);
    mainPane->set_start_child(*mainSidebar);
    //mainPane->append(*mainSidebar);

    selectedTrackID = static_cast<int>(conf->getCurrentTrackID());
    lastTrackID = selectedTrackID;
    stopBegin = 0;
    stopEnd = conf->getTrack(selectedTrackID).getStations().size() - 1;

    //trackStack->add(*startButton, "launch", "launch");
    mainPane->set_end_child(*trackStack);
    //mainPane->append(*trackStack);

    auto trackCount = conf->getTrackCount();
    for(uint64_t i = 0; i < trackCount;i++){
        const auto& track = conf->getTrack(i);

        const auto& stations = track.getStations();

        //create the lauch button for this track
        auto startButton = Gtk::make_managed<Gtk::Button>("Start Simulator");
        startButton->signal_clicked().connect([this, i](){
            try{
                get_application()->mark_busy();

                for(auto i = asyncTrackLoads.begin(); i < asyncTrackLoads.end(); i++){
                    if(i->valid()){
                        i->wait();
                        i = asyncTrackLoads.erase(i);
                    }
                }
                conf->selectTrack(i);
                const auto& stops = conf->getCurrentTrack().getStations();
                conf->getTrack(selectedTrackID).setFirstLocation(stops[stopBegin].position());
                conf->getTrack(selectedTrackID).setLastLocation(stops[stopEnd].position());
                sim = std::make_unique<simulator>(conf, input, get_application(), *this);
                get_application()->unmark_busy();
            }catch(const std::exception& e){
                Helper::print_exception(e);
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
    return selectedTrackID;
}

std::pair<int, int> mainMenu::getStopIDs() const {
    return {stopBegin, stopEnd};
}



simulator::~simulator(){
    hasError = true;

    
    //imguiHandler::removeSettingsTab("simulator");
    
    std::cout << std::endl;
    std::cout << "closing the simulator" << std::endl;
    simulatorGroup.reset();
    for(auto id:callbackIDs){
        input->removeEventCallback(id);
    }
    input->resetFlags();
    
    std::cout << "   waiting for physics thread to end" << std::endl;
    if(physicsLoop.valid()){
        physicsLoop.wait();
        physicsLoop.get();
    }
    //std::cout << "   destroying snowfx" << std::endl;
    //snow.reset();
    //std::cout << "   destroying status window" << std::endl;
    //statusWindow.reset();
    //std::cout << "   destroying videoManager" << std::endl;
    //video->close();
    std::cout << "   destroying physics" << std::endl;
    phy.reset();
    std::cout << "   destroying input_handler" << std::endl;
    input.reset();
    
    std::cout << "simulator has exited" << std::endl;
}

simulator::simulator(
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> _settings,
    std::shared_ptr<libtrainsim::control::input_handler> _input,
    Glib::RefPtr<Gtk::Application> _mainApp,
    mainMenu& _mainMenu
):
    settings{_settings},
    input{_input},
    mainApp{_mainApp},
    track{settings->getCurrentTrack()},
    mmainMenu{_mainMenu}
{
    
    //load the physics
    try{
        phy = std::make_unique<libtrainsim::physics>(settings->getCurrentTrack());
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error initializing physics"));
    }
    
    //create the empty window
    try{
        simulatorGroup = Gtk::WindowGroup::create();

        video = Gtk::make_managed<videoManager>(settings);
        simulatorGroup->add_window(*video);

        input->registerWindow(*video);
        auto id = input->addEventCallback(sigc::mem_fun(*video, &videoManager::handleEvents));
        callbackIDs.emplace_back(id);
        input->Keymap().add(GDK_KEY_F10, "MAXIMIZE");

        video->signal_close_request().connect([this](){
            mainApp->add_window(mmainMenu);
            mmainMenu.set_visible(true);
            return false;
        },false);
        //if(enableSnow){
        //    video->addTexture(imguiHandler::getDarkenTexture(backgroundDim));
        //}
    }catch(const std::exception& e){
        std::throw_with_nested(std::runtime_error("Could not create simulator window"));
    }



    //display all windows in the group
    for(auto win:simulatorGroup->list_windows()){
        mainApp->add_window(*win);
        win->set_visible(true);
    }
    
    /*
    //load the status display
    try{
        statusWindow = std::make_unique<libtrainsim::extras::statusDisplay>();
        statusWindow->changeBeginPosition(track.firstLocation());
        statusWindow->changeEndPosition(track.lastLocation());
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create status window"));
    }

    //create the snow fx layer
    try{
        snow = std::make_unique<libtrainsim::extras::snowFx>(settings);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not load snowFx"));
    }
    */
    
    //load the input system
    try{
        input->startSimulation();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Cannot create input_handler"));
    }

    hasError = false;
    
    //start the physics and video decode updates in a separate thread
    physicsLoop = std::async(std::launch::async, [this](){
        auto last_time = libtrainsim::core::Helper::now();

        do{
            updatePhysics();

            if(libtrainsim::core::Helper::now() - last_time < 5ms){
                std::this_thread::sleep_until(last_time + 5ms);
            }
            last_time = libtrainsim::core::Helper::now();
        }while(!hasErrored());

    });

    updateLoop = std::async(std::launch::async, [this](){do{update();}while(!hasErrored());});
    
    //add the settings page after everything else has been fully added
    //imguiHandler::addSettingsTab(std::make_shared<simulatorConfigMenu>(*this));
}

void simulator::updatePhysics(){

    phy->setSpeedlevel(input->getSpeedAxis());

    if(input->emergencyFlag()){
        phy->emergencyBreak();
    }

    phy->tick();
    auto loc = phy->getLocation();

    //check if the simulator has to be closed
    if(video->getDecoder().reachedEndOfFile() || phy->reachedEnd()){
        end();
        return;
    }

    //get the next frame that will be displayed
    auto frame_num = track.data().getFrame(loc);

    //if there is already a frame that is being rendered
    //then this call will buffer the furthest frame to be rendered
    video->gotoFrame(frame_num);
}

void simulator::update(){
    static auto last_time = libtrainsim::core::Helper::now();
    //actually render all of the windows
    try{
        //imguiHandler::startRender();

        //statusWindow->draw();

        if(enableSnow){
            //snow->updateTexture();
        }

        //video->refreshWindow();

        //imguiHandler::endRender();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error rendering the windows"));
    }

    //display statistics (speed, location, frametime, etc.)
    auto next_time = libtrainsim::core::Helper::now();

    time_si frametime = unit_cast(next_time-last_time, multiplier(std::milli::type{}));
    //statusWindow->appendFrametime(frametime);

    auto renderTimes = video->getNewRendertimes();
    if(renderTimes.has_value()){
        for(auto time:renderTimes.value()){
            //statusWindow->appendRendertime(time);
        }
    }

    //statusWindow->changePosition(phy->getLocation());

    //statusWindow->setAcceleration(phy->getAcceleration());
    //statusWindow->setSpeedLevel(Speedlevel);

    auto vel = phy->getVelocity();
    //snow->updateTrainSpeed(vel);
    //statusWindow->setVelocity(vel);

    while(Helper::now()-last_time < 8ms){
        std::this_thread::sleep_until(last_time+8ms);
    }

    last_time = Helper::now();

    input->update();

    if(input->closingFlag()){
        std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
        video->close();
    }
}

void simulator::end(){
    std::scoped_lock lock{errorMutex};
    hasError = true;
}

bool simulator::hasErrored(){
    std::shared_lock lock{errorMutex};
    return hasError;
}
