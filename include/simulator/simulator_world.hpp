#pragma once

#include "redhand/all.hpp"
#include "redhand/game_object.hpp"

class simulator_world : public redhand::complex_world{
    public:
        int onCreate(redhand::event<redhand::engine> evt);

        void tick(redhand::game_loop_event evt);
};

class simulator_screen : public redhand::game_object{
    public:
        simulator_screen();
};

int processGlobalInput(redhand::game_loop_event evt);