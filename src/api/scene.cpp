#include "scene.hpp"

#include <iostream>

void Scene::enter() {
    if (this->fEnter.valid()) {
        auto result = this->fEnter(this);

        if (!result.valid()) {
            sol::error err = result;
            std::cerr << err.what() << "\n";
        }
    }
}

void Scene::exit() {
    if (this->fExit.valid()) {
        auto result = this->fExit(this);

        if (!result.valid()) {
            sol::error err = result;
            std::cerr << err.what() << "\n";
        }
    }
}

void Scene::update(float dt) {
    if (this->fUpdate.valid()) {
        auto result = this->fUpdate(this, dt);

        if (!result.valid()) {
            sol::error err = result;
            std::cerr << err.what() << "\n";
        }
    }
}

void Scene::draw() {
    if (this->fDraw.valid()) {
        auto result = this->fDraw(this);

        if (!result.valid()) {
            sol::error err = result;
            std::cerr << err.what() << "\n";
        }
    }
}
