#include "video.hpp"
#include "control.hpp"
#include "simulator_config.hpp"

#include <future>
#include <memory>
#include <cassert>

#include "simulator.hpp"

int main(int argc, char **argv){
    
    //set a flag to display opengl errors on linux
    #ifdef setenv
        setenv( "MESA_DEBUG", "", 0 );
        setenv( "mesa_glthread", "true", 1 );
    #endif
    
    //print the individual cmd arguments to make debugging them easier
    std::cout << "command line args:" << std::endl;
    for (int i = 0; i < argc;i++){
        std::cout << argv[i] << std::endl;
    }

    //check if the libtrainsim version is high enough
    const libtrainsim::core::version required_version{0,11,0};
    assert((libtrainsim::core::lib_version >= required_version) && "libtrainsim version not high enogh!");

    //load the simulator configuration
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;
    try{
        std::filesystem::path config_loc = argc > 1 ? argv[1] : "data/production_data/simulator.json";
        conf = std::make_shared<libtrainsim::core::simulatorConfiguration>(config_loc, true);
    }catch(const std::exception& e){
        libtrainsim::core::Helper::print_exception(e);
        return 100;
    }
   
    //load the input handler with serial input if available
    std::shared_ptr<libtrainsim::control::input_handler> input;
    try{
        input = std::make_shared<libtrainsim::control::input_handler>(conf->getSerialConfigLocation());
    }catch(const std::exception& e){
        libtrainsim::core::Helper::print_exception(e);
        return 100;
    }
    
    std::unique_ptr<simulator> sim;
    int selectedTrackID = static_cast<int>(conf->getCurrentTrackID());
    int lastTrackID = selectedTrackID;
    int stopBegin = 0;
    int stopEnd = 0;
    std::vector<std::future<void>> asycTrackLoads;
    
    try{
        libtrainsim::Video::imguiHandler::loadShaders(conf->getShaderLocation(),conf->getTextureLocation());
    }catch(const std::exception& e){
        libtrainsim::core::Helper::print_exception(e);
        return 100;
    }
    
    while(!input->closingFlag()){
        if(sim == nullptr){
            input->update();
            //output main menu
            libtrainsim::Video::imguiHandler::startRender();
                ImGui::Begin(
                    "Main Menu", 
                    NULL, 
                    ImGuiWindowFlags_NoCollapse
                );
                    ImGui::BeginTable(
                        "main menu cols", 
                        3, 
                        (ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp) & (~ImGuiTableFlags_Sortable)
                    );
                    
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
                                [&conf,&selectedTrackID](){
                                    auto ID = selectedTrackID;
                                    conf->getTrack(ID).ensure();
                                })
                            );
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
                    
                    if(ImGui::Button("Start Simulator")){
                        try{
    
                            //wait for all tracks to finish loading before starting the track
                            for(auto& task:asycTrackLoads){
                                if(task.valid()){
                                    task.wait();
                                    task.get();
                                }
                            }

                            conf->selectTrack(selectedTrackID);
                            const auto& stops = conf->getCurrentTrack().getStations();
                            conf->getTrack(selectedTrackID).setFirstLocation(stops[stopBegin].position());
                            conf->getTrack(selectedTrackID).setLastLocation(stops[stopEnd].position());
                            sim = std::make_unique<simulator>(conf);
                        }catch(const std::exception& e){
                            libtrainsim::core::Helper::print_exception(e);
                        }
                    }
                
                ImGui::End();
                
                
            libtrainsim::Video::imguiHandler::endRender();
            
        }else{
            //update the output image in the current thread
            while(!sim->hasErrored()){
                input->update();
                sim->serial_speedlvl(input->getSpeedAxis());
                    
                if(input->emergencyFlag()){
                    sim->emergencyBreak();
                }
                
                if(input->closingFlag()){
                    std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
                    sim->end();
                }
                sim->updateImage();
            };
            
            sim.reset();
        }
        
        
    }
    
    //make sure no track is being loaded while the program closes
    for(auto& task:asycTrackLoads){
        if(task.valid()){
            task.wait();
            task.get();
        }
    }

    input.reset();

    return 0;
}
