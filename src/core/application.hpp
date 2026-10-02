#pragma once
#include "api.hpp"

#include <sol/sol.hpp>

class Application {
public:
    void run();
    void init();
    void loop();
    void quit();

private:
    sol::state lua;
    API api;
};
