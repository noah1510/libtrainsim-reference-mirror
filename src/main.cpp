#include "simulator/simulator_world.hpp"

#include "video.hpp"
#include "control.hpp"

int main( int argc, char **argv){
    int exitCode = 0;
    if (argc == 0){
        return 100;
    }

    //create the engine object
    auto gameEngine = std::make_shared<redhand::engine>(argv[0]);

    //get the current config of the engine
    redhand::engine_config conf = gameEngine->getConfig();

    //change the configuration and set the new config
    conf.title = "Bahn Simulator";
    conf.RESIZABLE = true;
    conf.window_height = 720;
    conf.window_width = 1280;
    gameEngine->setConfig(conf);

    //checking if libtrainsim works as correctly
    libtrainsim::control::hello();
    libtrainsim::video::hello();

    //set the function which handles all the controls and physics computation
    gameEngine->addGameLoopHandler(processGlobalInput, "global_input");

    //initilize the game engine
    gameEngine->init();

    gameEngine->changeWorld(std::shared_ptr<redhand::complex_world>(new simulator_world()));

    //set the exit flags in case something went wrong 
    exitCode = gameEngine->getErrorCode();
    if(exitCode < 0){
        gameEngine->stopGame();
    }else{        
        //run the game
        exitCode = gameEngine->runGame(); 
    }
    
    //return the error code if something bad happened or 0 if everything is fine
    return abs(exitCode);
    
}
