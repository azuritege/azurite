#pragma once
#include <string>

namespace Input {
    bool keyDown(std::string name);
    bool keyPressed(std::string name);
    bool keyReleased(std::string name);
    int keyAxis(std::string positive, std::string negative);
}