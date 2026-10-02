#include "application.hpp"

#include <filesystem>
#include <raylib.h>
#include <string>

void Application::run() {
    init();
    while (!WindowShouldClose()) {
        loop();
    }
    quit();
}

void Application::init() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    InitWindow(1024, 576, "Azurite");
    InitAudioDevice();

    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);

    api.inject(lua);

    std::filesystem::path main = std::filesystem::path(GetApplicationDirectory()) / "main.lua";
    auto result = lua.safe_script_file(main.string(), sol::script_pass_on_error);

    if (!result.valid()) {
        sol::error error = result;
        TraceLog(LOG_ERROR, "Lua error: %s", error.what());
    }
}

void Application::loop() {
    api.update(GetFrameTime());
    api.draw();
}

void Application::quit() {
    CloseAudioDevice();
    CloseWindow();
}