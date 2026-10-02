#pragma once
#include <sol/sol.hpp>

class Scene {
public:
    sol::protected_function fEnter;
    sol::protected_function fDraw;
    sol::protected_function fUpdate;
    sol::protected_function fExit;

    sol::table fields;

    void enter();
    void update(float dt);
    void draw();
    void exit();
};