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

    selectedTrackID = static_cast<int>(conf->getCurrentTrackID());
    lastTrackID = selectedTrackID;
    stopBegin = 0;
    stopEnd = conf->getTrack(selectedTrackID).getStations().size() - 1;

    startButton = Gtk::make_managed<Gtk::Button>("Start Simulator");
    startButton->signal_clicked().connect([this](){
        try{
            sim = std::make_unique<simulator>(conf, input, get_application(), *this);
        }catch(const std::exception& e){
            Helper::print_exception(e);
            close();
            return;
        }
        hide();
    });
    set_child(*startButton);

}


mainMenu::~mainMenu(){
    finishTrackLoad();
}

void mainMenu::finishTrackLoad() {
    //wait for all tracks to finish loading before starting the track
    for(auto& task:asycTrackLoads){
        if(task.valid()){
            task.wait();
            task.get();
        }
    }
    asycTrackLoads.clear();
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


/*void mainMenu::content() {
    if(ImGui::BeginTable(
        "main menu cols", 
        3, 
        (ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp) & (~ImGuiTableFlags_Sortable)
    )){
    
        //display the titles in row 0
        ImGui::TableNextColumn();
        ImGui::Text("Track selection");
        
        ImGui::TableNextColumn();
        ImGui::Text("Track begin selection");
        
        ImGui::TableNextColumn();
        ImGui::Text("Track end selection");
        
        //diplay the list of tracks
        ImGui::TableNextColumn();
        auto trackCount = conf->getTrackCount();
        for(uint64_t i = 0; i < trackCount;i++){
            const auto& track = conf->getTrack(i);
            ImGui::RadioButton(track.getName().c_str(),&selectedTrackID,i);
        }
        
        if(lastTrackID != selectedTrackID){
            asycTrackLoads.emplace_back(std::async(
                std::launch::async, 
                [this](){
                    auto ID = selectedTrackID;
                    conf->getTrack(ID).ensure();
                })
            );
            
            stopBegin = 0;
            stopEnd = conf->getTrack(selectedTrackID).getStations().size() - 1;
            lastTrackID = selectedTrackID;
        }
        
        //display where all of the stops where it is possible to begin
        try{
            ImGui::TableNextColumn();
            const auto& stops = conf->getTrack(selectedTrackID).getStations();
            for(uint64_t i = 0; i < stops.size()-1; i++){
                std::stringstream ss;
                ss << stops[i].name() << "";
                ImGui::RadioButton(ss.str().c_str(),&stopBegin,i);
            }
            
            //display where all of the stops where it is possible to end
            ImGui::TableNextColumn();
            for(uint64_t i = stopBegin+1; i < stops.size();i++){
                std::stringstream ss;
                ss << stops[i].name() << " ";
                ImGui::RadioButton(ss.str().c_str(), &stopEnd, i);
            }
            
            if(stopEnd <= stopBegin){
                stopEnd=stopBegin+1;
            }
        }catch(const std::exception& e){
            libtrainsim::core::Helper::print_exception(e);
        }
        ImGui::EndTable();
    }
        
    //Pressing the button switches to the other half of the if.
    //This prevents double pressing the start button which
    if(ImGui::Button("Start Simulator")){
        ShouldStart = true;
    }
        
}*/



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
