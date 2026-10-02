#pragma once
#include "api/scene.hpp"

#include <memory>
#include <sol/sol.hpp>

class API {
public:
    void inject(sol::state& lua);

    void update(float dt);
    void draw();
    void quit();

private:
    std::shared_ptr<Scene> curScene = nullptr;
};
