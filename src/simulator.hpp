#pragma once

#include <filesystem>
#include <shared_mutex>
#include "guard.hpp"
#include "video.hpp"
#include "track_configuration.hpp"
#include "physics.hpp"
#include <future>

class simulator{
    private:
        guardedVar<unsigned int> currentFrame = guardedVar<unsigned int>(0);
        guardedVar<bool> hasError = guardedVar<bool>(true);

        sakurajin::unit_system::common::acceleration acceleration;

        long double Speedlevel;

        

        const std::string window_name = " bahn_simulator";
        bool updateImage();

        std::future<void> graphicsLoop;

        libtrainsim::core::Track track;
        libtrainsim::physics phy;

    public:
        simulator(const libtrainsim::core::Track& dat);
        ~simulator();
        bool hasErrored();
        void end();

        void accelerate();
        void decellerate();

};
