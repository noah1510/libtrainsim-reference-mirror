#pragma once


/**
 * @brief An input event to signal the start of the simulator.
 */
class simulatorStartEvent {
  public:
    size_t trackIndex     = 0;
    size_t stopBeginIndex = 0;
    size_t stopEndIndex   = 0;

    /**
     * @brief Construct a new input object with the given parameters.
     * It creates a normal input event that can be parsed back into this object using simulatorStartEvent::parse.
     * @param _trackID    The index of the track to start the simulator on.
     * @param _stopBeginID The index of the first stop to start the simulator on.
     * @param _stopEndID  The index of the last stop to start the simulator on.
     */
    [[maybe_unused]]
    static SimpleGFX::inputEvent create(size_t _trackIndex, size_t _stopBeginIndex, size_t _stopEndIndex){
        std::stringstream ss;
        ss << _trackIndex << ";" << _stopBeginIndex << ";" << _stopEndIndex;
        return SimpleGFX::inputEvent{ss.str(), "simulatorStartEvent"};
    };

    /**
     * @brief Parse an input into a simulatorStartEvent.
     * This is used to convert an inputEvent back to a simulatorStartEvent.
     * This only has a value if the inputEvent was created by simulatorStartEvent::create.
     * @param event The inputEvent to convert.
     */
    [[maybe_unused]]
    static std::optional<simulatorStartEvent> parse(const SimpleGFX::inputEvent& event){
        if (event.originName != "simulatorStartEvent") {
            return {};
        }

        auto split = SimpleGFX::string::splitString(event.name, ';');
        if (split.size() != 3) {
            return {};
        }

        simulatorStartEvent result;
        result.trackIndex     = std::stoull(split[0]);
        result.stopBeginIndex = std::stoull(split[1]);
        result.stopEndIndex   = std::stoull(split[2]);

        return result;
    };
};

/**
 * @brief An input event to signal the stop of the simulator.
 */
class simulatorStopEvent {
  public:
    /**
     * @brief Construct a new input object with the given parameters.
     * It creates a normal input event that can be parsed back into this object using simulatorStopEvent::parse.
     * @param _trackID    The index of the track to start the simulator on.
     * @param _stopBeginID The index of the first stop to start the simulator on.
     * @param _stopEndID  The index of the last stop to start the simulator on.
     */
    [[maybe_unused]]
    static SimpleGFX::inputEvent create(){
        return SimpleGFX::inputEvent{"simulatorStopEvent", "simulatorStopEvent"};
    };

    /**
     * @brief Parse an input into a simulatorStopEvent.
     * This is used to convert an inputEvent back to a simulatorStartEvent.
     * This only has a value if the inputEvent was created by simulatorStartEvent::create.
     * @param event The inputEvent to convert.
     */
    [[maybe_unused]]
    static std::optional<simulatorStopEvent> parse(const SimpleGFX::inputEvent& event){
        if (event.originName != "simulatorStopEvent") {
            return {};
        }

        if (event.name != "simulatorStopEvent") {
            return {};
        }

        return simulatorStopEvent{};
    };
};
