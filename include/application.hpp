#pragma once
#include <string>
#include <filesystem>

#include <raylib.h>
#include <sol/sol.hpp>

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
        InitWindow(1024, 576, "Azurite");
        SetTargetFPS(60);

        lua.open_libraries(
            sol::lib::base,
            sol::lib::package,
            sol::lib::math,
            sol::lib::table
        );

        std::filesystem::path main = std::filesystem::path(GetApplicationDirectory()) / "main.lua";
        lua.safe_script_file(main.string());
    }

    void loop() {
        BeginDrawing();
            ClearBackground(BLACK);
        EndDrawing();
    }

    void quit() {
        CloseWindow();
    }
};