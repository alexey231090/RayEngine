#pragma once
#include <string>

namespace REngine {

class ProcessRunner {
public:
    // Launches the game as a separate standalone window/process
    static bool LaunchGameProcess(const std::string& scenePath = "scene.json");

    // Returns true if the child game process is currently running
    static bool IsGameRunning();

    // Stops and terminates the child game process
    static void StopGameProcess();

    // Returns the path of the current executable
    static std::string GetExecutablePath();
};

} // namespace REngine
