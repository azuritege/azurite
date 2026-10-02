#include "input.hpp"

#include <cctype>
#include <raylib.h>
#include <unordered_map>

namespace Input {
    int getKeyFromString(std::string name) {
        for (char& c : name) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        if (name.size() == 1) {
            char c = name[0];

            if (c >= 'a' && c <= 'z')
                return KEY_A + (c - 'a');
            if (c >= '0' && c <= '9')
                return KEY_ZERO + (c - '0');
        }

        static const std::unordered_map<std::string, int> keys = {
            {"space", KEY_SPACE},         {"enter", KEY_ENTER},
            {"escape", KEY_ESCAPE},       {"tab", KEY_TAB},
            {"backspace", KEY_BACKSPACE}, {"left", KEY_LEFT},
            {"right", KEY_RIGHT},         {"up", KEY_UP},
            {"down", KEY_DOWN},           {"shift", KEY_LEFT_SHIFT},
            {"ctrl", KEY_LEFT_CONTROL},   {"alt", KEY_LEFT_ALT}};

        auto it = keys.find(name);
        return it != keys.end() ? it->second : KEY_NULL;
    }

    bool keyDown(std::string name) {
        return IsKeyDown(getKeyFromString(name));
    }

    bool keyPressed(std::string name) {
        return IsKeyPressed(getKeyFromString(name));
    }

    bool keyReleased(std::string name) {
        return IsKeyReleased(getKeyFromString(name));
    }

    int keyAxis(std::string positive, std::string negative) {
        return keyDown(positive) - keyDown(negative);
    }
}