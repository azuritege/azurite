#pragma once
#include <sol/sol.hpp>

class Scene {
public:
    sol::protected_function fEnter;
    sol::protected_function fDraw;
    sol::protected_function fUpdate;
    sol::protected_function fExit;

    sol::table fields;

    void enter() {
        if (this->fEnter.valid()) {
            auto result = this->fEnter(this);

            if (!result.valid()) {
                sol::error err = result;
                std::cerr << err.what() << "\n";
            }
        }
    }

    void exit() {
        if (this->fExit.valid()) {
            auto result = this->fExit(this);

            if (!result.valid()) {
                sol::error err = result;
                std::cerr << err.what() << "\n";
            }
        }
    }

    void update(float dt) {
        if (this->fUpdate.valid()) {
            auto result = this->fUpdate(this, dt);

            if (!result.valid()) {
                sol::error err = result;
                std::cerr << err.what() << "\n";
            }
        }
    }

    void draw() {
        if (this->fDraw.valid()) {
            auto result = this->fDraw(this);

            if (!result.valid()) {
                sol::error err = result;
                std::cerr << err.what() << "\n";
            }
        }
    }
};