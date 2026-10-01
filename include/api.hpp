#pragma once
#include <raylib.h>
#include <sol/sol.hpp>

#include <memory>
#include <utility>

#include <cstdint>

#include <core/audio.hpp>
#include <core/graphics.hpp>
#include <core/input.hpp>
#include <core/math.hpp>
#include <core/scene.hpp>

std::shared_ptr<Scene> curScene = nullptr;

void luaInject(sol::state &lua) {
    auto a = lua["a"].get_or_create<sol::table>();

    a.new_usertype<Scene>(
        "Scene",
        sol::no_constructor,

        "enter",  &Scene::fEnter,
        "draw",   &Scene::fDraw,
        "update", &Scene::fUpdate,
        "exit",   &Scene::fExit,

        sol::meta_function::index,
        [](Scene& scene, sol::object key) -> sol::object {
            return scene.fields.raw_get<sol::object>(key);
        },

        sol::meta_function::new_index,
        [](Scene& scene, sol::object key, sol::object value) {
            scene.fields.raw_set(key, value);
        }
    );

    a.set_function("scene", [&lua]() {
        auto scene = std::make_shared<Scene>();
        scene->fields = lua.create_table();
        return scene;
    });

    a.new_usertype<LuaTexture>("Texture", sol::constructors<LuaTexture(std::string)>());
    a.new_usertype<LuaMusic>("Music", sol::constructors<LuaMusic(std::string)>());
    a.new_usertype<LuaSound>("Sound", sol::constructors<LuaSound(std::string)>());

    a.set_function("go", [](std::shared_ptr<Scene> scene) {
        if (!scene) return;

        if (curScene) {
            curScene->exit();
        }

        curScene = std::move(scene);
        curScene->enter();
    });

    a.set_function("clear", [](uint8_t r, uint8_t g, uint8_t b) {
        ClearBackground(Color{r, g, b, 255});
    });

    a.set_function("load_texture", [](std::string path) {
        return loadTexture(path);
    });

    a.set_function("draw_texture", &drawTexture);

    a.set_function("key_down", [](std::string name) {
        return keyDown(name);
    });

    a.set_function("key_pressed", [](std::string name) {
        return keyPressed(name);
    });

    a.set_function("key_released", [](std::string name) {
        return keyReleased(name);
    });

    a.set_function("key_axis", [](std::string positive, std::string negative) {
        return keyAxis(positive, negative);
    });

    a.set_function("load_sound", &loadSound);
    a.set_function("play_sound", &playSound);
    a.set_function("stop_sound", &stopSound);

    a.set_function("load_music", &loadMusic);
    a.set_function("play_music", &playMusic);
    a.set_function("update_music", &updateMusic);
    a.set_function("stop_music", &stopMusic);

    a.set_function("calc_lerp", &lerp);
    a.set_function("calc_clamp", &clamp);
    a.set_function("calc_step", &step);
    a.set_function("calc_sign", &sign);
}