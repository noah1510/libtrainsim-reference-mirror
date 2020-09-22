#include <filesystem>
#include <shared_mutex>
#include "guard.hpp"
#include "video.hpp"
#include <future>

class simulator{
    private:
        guardedVar<unsigned int> currentFrame = guardedVar<unsigned int>(0);
        guardedVar<bool> hasError = guardedVar<bool>(true);

        const std::string window_name = " bahn_simulator";
        bool updateImage();

        std::future<void> graphicsLoop;

    public:
        simulator(std::filesystem::path URI);
        ~simulator();
        void nextFrame();
        bool hasErrored();

};