#pragma once

#include <filesystem>
#include <shared_mutex>
#include "guard.hpp"
#include "video.hpp"
#include "track_configuration.hpp"
#include "physics.hpp"
#include "serialcontrol.hpp"
#include "helper.hpp"
#include "statusDisplay.hpp"
#include "simulator_config.hpp"
#include "SDL2_framerate.h"
#include <future>

class simulator{
    private:
        guardedVar<unsigned int> currentFrame = guardedVar<unsigned int>(0);
        guardedVar<bool> hasError = guardedVar<bool>(true);

        libtrainsim::core::input_axis Speedlevel;

        std::future<void> graphicsLoop;

        const libtrainsim::core::Track& track;
        libtrainsim::physics phy;

        libtrainsim::Video::videoManager video;
        libtrainsim::extras::statusDisplay statusWindow;
        FPSmanager fpsControl;

    public:
        simulator(std::shared_ptr<libtrainsim::core::simulatorConfiguration> settings);
        ~simulator();
        bool hasErrored();
        void end();
        
        void emergencyBreak();
        bool updateImage();

        void serial_speedlvl(libtrainsim::core::input_axis Slvl);
};
