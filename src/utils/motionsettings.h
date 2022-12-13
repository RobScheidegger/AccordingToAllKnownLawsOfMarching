#pragma once

#include <string>

/**
 * @brief The MotionSettings struct represents the settings governing how much
 */
struct MotionSettings {
    /**
     * @brief enabled determines whether or not the motion should be enabled for the scene (false renders a static output).
     */
    bool enabled = false;
    /**
     * @brief fps Determines the number of frames per second to render
     */
    int fps = 30;
    /**
     * @brief seconds Determines the number of seconds to render for
     */
    int seconds = 5;
    /**
     * @brief cleanup Determines whether or not to clean intermediate frame files (used for testing).
     */
    bool cleanup = true;
    /**
     * @brief output The path of the file to output to
     */
    std::string output;
};

extern MotionSettings motionSettings;
