#pragma once
#include <string>
#include <filesystem>

#include <raylib.h>
#include <sol/sol.hpp>

#include <api.hpp>

class Application
{
private:
    sol::state lua;

public:
    void run() {
        init();
        while (!WindowShouldClose()) {
            loop();
        }
        quit();
    }

    void init() {
        SetConfigFlags(FLAG_WINDOW_HIGHDPI);

        InitWindow(1024, 576, "Azurite");
        InitAudioDevice();
        
        SetExitKey(KEY_NULL);
        SetTargetFPS(60);

        lua.open_libraries(
            sol::lib::base,
            sol::lib::package,
            sol::lib::math,
            sol::lib::table
        );

        luaInject(lua);

        std::filesystem::path main = std::filesystem::path(GetApplicationDirectory()) / "main.lua";
        auto result = lua.safe_script_file(main.string(), sol::script_pass_on_error);

        if (!result.valid()) {
            sol::error error = result;
            TraceLog(LOG_ERROR, "Lua error: %s", error.what());
        }
    }

    void loop() {
        if (curScene) curScene->update(GetFrameTime());

        BeginDrawing();
            ClearBackground(BLACK);
            if (curScene) curScene->draw();
        EndDrawing();
    }

    void quit() {
        if (curScene) {
            curScene->exit();
            curScene.reset();
        }

        CloseAudioDevice();
        CloseWindow();
    }
};