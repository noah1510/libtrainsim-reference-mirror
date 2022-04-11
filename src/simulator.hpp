#pragma once

#include <filesystem>
#include <shared_mutex>
#include "guard.hpp"
#include "video.hpp"
#include "track_configuration.hpp"
#include "physics.hpp"
#include "serialcontrol.hpp"
#include <future>

class simulator{
    private:
        guardedVar<unsigned int> currentFrame = guardedVar<unsigned int>(0);
        guardedVar<bool> hasError = guardedVar<bool>(true);

        libtrainsim::core::input_axis Speedlevel;



        const std::string window_name = " bahn_simulator";
        bool updateImage();
        void updateSerial();

        std::future<void> graphicsLoop;
        std::future<void> serialcontrolLoop;

        libtrainsim::core::Track track;
        libtrainsim::physics phy;


    public:
        simulator(const libtrainsim::core::Track& dat);
        ~simulator();
        bool hasErrored();
        void end();

        void accelerate();
        void decellerate();

        void serial_speedlvl(libtrainsim::core::input_axis Slvl);
};
