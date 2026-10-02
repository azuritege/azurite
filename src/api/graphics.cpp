#include "graphics.hpp"

#include <algorithm>

namespace Graphics {
    void clear(uint8_t r, uint8_t g, uint8_t b) {
        ClearBackground(Color{r, g, b, 255});
    }

    int screen_width() {
        return GetScreenWidth();
    }

    int screen_height() {
        return GetScreenHeight();
    }

    LuaTexture::LuaTexture(std::string path) {
        Image image = LoadImage((std::string(GetApplicationDirectory()) + path).c_str());
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    LuaTexture loadTexture(std::string path) {
        LuaTexture texture{path};
        return texture;
    }

    void Renderer::flush() {
        std::sort(queue.begin(), queue.end(),
                [](const RenderItem& a, const RenderItem& b) { return a.zIndex < b.zIndex; });

        for (const auto& item : queue) {
            DrawTextureEx(item.texture, item.position, item.rotation, item.scale, WHITE);
        }

        queue.clear();
    }

    void Renderer::pushTexture(LuaTexture texture, float x, float y, int zIndex, float rotation,
                            float scale) {
        queue.push_back({texture.texture, {x, y}, rotation, scale, zIndex});
    }
}