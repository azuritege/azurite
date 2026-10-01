#pragma once
#include <raylib.h>

#include <string>

struct LuaTexture {
    Texture2D texture;

    LuaTexture(const std::string &name) {
        Image image = LoadImage((std::string(GetApplicationDirectory()) + name).c_str());
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }
};

LuaTexture loadTexture(std::string name) {
    LuaTexture texture{name};
    return texture;
}

void drawTexture(LuaTexture texture, float x, float y, float rotation = 0.0f, float scale = 1.0f) {
    DrawTextureEx(texture.texture, Vector2{x, y}, rotation, scale, WHITE);
}