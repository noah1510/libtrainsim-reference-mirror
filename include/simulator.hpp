#pragma once

#include "simulator_config.hpp"
#include "video.hpp"
#include "physics.hpp"

#include "statusDisplay.hpp"
#include "snowFx.hpp"

class simulator{
    private:
        bool hasError = true;
        std::shared_mutex errorMutex;

        libtrainsim::core::input_axis Speedlevel;

        std::future<void> physicsLoop;

        const libtrainsim::core::Track& track;
        std::unique_ptr<libtrainsim::physics> phy;

        std::unique_ptr<libtrainsim::Video::videoManager> video;
        std::unique_ptr<libtrainsim::extras::statusDisplay> statusWindow;
        std::unique_ptr<libtrainsim::extras::snowFx> snow;

        bool enableSnow = false;
        int backgroundDim = 20;

    public:
        simulator(std::shared_ptr<libtrainsim::core::simulatorConfiguration> settings);
        ~simulator();
        bool hasErrored();
        void end();
        
        void emergencyBreak();
        bool updateImage();

        void serial_speedlvl(libtrainsim::core::input_axis Slvl);
};
