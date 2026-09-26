#include "core/Application.hpp"
#include <iostream>
#include <string>

void PrintHelp() {
    std::cout << "REngine -- AI-First 3D Game Engine & Editor\n\n"
              << "Usage: RaylibEngineApp.exe [options]\n\n"
              << "Options:\n"
              << "  --editor            Run engine in Editor mode (default)\n"
              << "  --game              Run standalone Game window (Godot style)\n"
              << "  --scene <path>      Specify scene JSON file to load (default: scene.json)\n"
              << "  --clean-scene       Clear demo shapes from scene (preserve camera)\n"
              << "  --test-frames <N>   Run engine for N frames then exit cleanly (ideal for AI automated testing)\n"
              << "  --headless          Run with hidden window (ideal for CI / background agent tests)\n"
              << "  --help, -h          Show this help message\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    REngine::AppConfig config;
    config.width = 1280;
    config.height = 720;
    config.title = "REngine Editor";

    bool cleanScene = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--game") {
            config.isGameMode = true;
        } else if (arg == "--editor") {
            config.isGameMode = false;
        } else if (arg == "--clean-scene") {
            cleanScene = true;
        } else if (arg == "--test-frames" || arg == "-tf") {
            if (i + 1 < argc) {
                config.testFrames = std::stoi(argv[++i]);
            }
        } else if (arg == "--headless") {
            config.headless = true;
        } else if (arg == "--scene") {
            if (i + 1 < argc) {
                config.scenePath = argv[++i];
            }
        } else if (arg == "--help" || arg == "-h") {
            PrintHelp();
            return 0;
        }
    }

    REngine::Application app(config);

    if (cleanScene) {
        app.ClearScene(true);
    }

    app.Run();

    return 0;
}
