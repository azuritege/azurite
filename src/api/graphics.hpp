#pragma once
#include <cstdint>
#include <raylib.h>
#include <string>
#include <vector>

namespace Graphics {
    void clear(uint8_t r, uint8_t g, uint8_t b);

    int screen_width();
    int screen_height();

    struct LuaTexture {
        Texture2D texture{};
        LuaTexture(std::string path);
    };

    LuaTexture loadTexture(std::string path);

    struct RenderItem {
        Texture2D texture{};
        Vector2 position{};
        float rotation = 0.0f;
        float scale = 1.0f;
        int zIndex = 0;
    };

    class Renderer {
    public:
        void pushTexture(LuaTexture texture, float x, float y, int zIndex = 0, float rotation = 0.0f,
                        float scale = 1.0f);
        void flush();

    private:
        std::vector<RenderItem> queue;
    };
}