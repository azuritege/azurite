#include "api.hpp"

#include "api/audio.hpp"
#include "api/graphics.hpp"
#include "api/input.hpp"
#include "api/math.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

Graphics::Renderer renderer;

void API::inject(sol::state& lua) {
    auto a = lua["a"].get_or_create<sol::table>();

    // Core
    auto core = a["core"].get_or_create<sol::table>();
    core.new_usertype<Scene>(
        "Scene", sol::no_constructor,

        "enter", &Scene::fEnter,
        "draw", &Scene::fDraw,
        "update", &Scene::fUpdate,
        "exit", &Scene::fExit,

        sol::meta_function::index,
        [](Scene& scene, sol::object key) -> sol::object {
            return scene.fields.raw_get<sol::object>(key);
        },

        sol::meta_function::new_index,
            [](Scene& scene, sol::object key, sol::object value) { scene.fields.raw_set(key, value);
        }
    );
    core.set_function("scene", [&]() {
        auto scene = std::make_shared<Scene>();
        scene->fields = lua.create_table();
        return scene;
    });
    core.set_function("go", [&](std::shared_ptr<Scene> scene) {
    if (!scene)
        return;

        if (curScene) {
            curScene->exit();
        }
        
        curScene = std::move(scene);
        curScene->enter();
    });
    
    // Graphics
    auto graphics = a["graphics"].get_or_create<sol::table>();
    graphics.new_usertype<Graphics::LuaTexture>("Texture", sol::constructors<Graphics::LuaTexture(std::string)>());

    graphics.set_function("clear", &Graphics::clear);
    graphics.set_function("load_texture", &Graphics::loadTexture);
    graphics.set_function(
        "draw_texture",
        sol::overload(
            [](Graphics::LuaTexture texture, float x, float y) {
                renderer.pushTexture(texture, x, y, 0, 0.0f, 1.0f);
            },
            [](Graphics::LuaTexture texture, float x, float y, int zIndex) {
                renderer.pushTexture(texture, x, y, zIndex, 0.0f, 1.0f);
            },
            [](Graphics::LuaTexture texture, float x, float y, int zIndex, float rotation, float scale) {
                renderer.pushTexture(texture, x, y, zIndex, rotation, scale);
            }
        )
    );
    graphics.set_function("screen_width", &Graphics::screen_width);
    graphics.set_function("screen_height", &Graphics::screen_height);

    // Input
    auto input = a["input"].get_or_create<sol::table>();
    input.set_function("key_down", &Input::keyDown);
    input.set_function("key_pressed", &Input::keyPressed);
    input.set_function("key_released", &Input::keyReleased);
    input.set_function("key_axis", &Input::keyAxis);

    // Audio
    auto audio = a["audio"].get_or_create<sol::table>();
    a.new_usertype<Audio::LuaMusic>("Music", sol::constructors<Audio::LuaMusic(std::string)>());
    a.new_usertype<Audio::LuaSound>("Sound", sol::constructors<Audio::LuaSound(std::string)>());

    audio.set_function("load_sound", &Audio::loadSound);
    audio.set_function("play_sound", &Audio::playSound);
    audio.set_function("stop_sound", &Audio::stopSound);

    audio.set_function("load_music", &Audio::loadMusic);
    audio.set_function("play_music", &Audio::playMusic);
    audio.set_function("update_music", &Audio::updateMusic);
    audio.set_function("stop_music", &Audio::stopMusic);

    // Math
    auto math = a["math"].get_or_create<sol::table>();
    math.set_function("lerp", &Math::lerp);
    math.set_function("clamp", &Math::clamp);
    math.set_function("step", &Math::step);
    math.set_function("sign", &Math::sign);
}

void API::update(float dt) {
    if (curScene)
        curScene->update(dt);
}

void API::draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    if (curScene)
        curScene->draw();
    renderer.flush();
    EndDrawing();
}

void API::quit() {
    if (curScene) {
        curScene->exit();
        curScene.reset();
    }
}
