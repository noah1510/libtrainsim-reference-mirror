#include "simulator_world.hpp"
#include "redhand/game_object.hpp"
#include "redhand/types.hpp"
#include <memory>
#include <shared_mutex>

using namespace redhand;

int simulator_world::onCreate(event<engine>){
    //Add textures to world

    //hand
    if( this->addObject(std::make_unique<simulator_screen>()) < 0){
        return -3;
    }
    
    return 0;
}

void simulator_world::tick(game_loop_event evt) {
    complex_world::tick(evt);

    //move the camera
    glm::vec2 deltaCamera = {0.0f,0.0f};

    if( input_system::static_isKeyPressed(KEY_D) ){
        deltaCamera.x = 0.02f;
    }else if( input_system::static_isKeyPressed(KEY_A) ){
        deltaCamera.x = -0.02f;
    }

    this->moveCamera(deltaCamera.x,0);

    return;
}

simulator_screen::simulator_screen():redhand::game_object(){
    game_object_properties conf;

    conf.postition = {-1.0,-1.0};
    conf.scale = {2.0,2.0};
    conf.points_coordinates = {
        {0.0,0.0},
        {0.0,1.0},
        {1.0,1.0},
        {1.0,0.0}
    };
    conf.triangle_indices = {
        {0,1,2},
        {0,2,3}
    };
    conf.point_colors = {
        {1,1,1},
        {1,1,1},
        {1,1,1},
        {1,1,1}
    };
    conf.automatic_scaling = true;
    

    std::shared_lock<std::shared_mutex> lock (mutex_object_properties);
    object_properties = conf;
    lock.unlock();

    updateBuffers();
    
}

int processGlobalInput(game_loop_event evt){

    if(evt.getRaiser() == nullptr){
        return -12;
    }

    if(input_system::static_isKeyPressed(KEY_ESCAPE)){
        evt.getRaiser()->stopGame();
    }

    return 0;

}