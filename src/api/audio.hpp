#pragma once
#include <raylib.h>
#include <string>
#include <utility>

namespace Audio {
    struct LuaSound {
        Sound sound{};

        LuaSound(const std::string& name);
        LuaSound(const LuaSound&) = delete;
        LuaSound(LuaSound&& other) noexcept;

        LuaSound& operator=(const LuaSound&) = delete;
        LuaSound& operator=(LuaSound&& other) noexcept;

        ~LuaSound();

    private:
        void release();
    };

    LuaSound loadSound(const std::string& name);
    void playSound(const LuaSound& sound);
    void stopSound(const LuaSound& sound);

    struct LuaMusic {
        Music music;

        LuaMusic(const std::string& name);
        LuaMusic(const LuaMusic&) = delete;
        LuaMusic(LuaMusic&& other) noexcept;

        LuaMusic& operator=(const LuaMusic&) = delete;
        LuaMusic& operator=(LuaMusic&& other) noexcept;

        ~LuaMusic();

    private:
        void release();
    };

    LuaMusic loadMusic(const std::string& name);
    void playMusic(const LuaMusic& music);
    void updateMusic(const LuaMusic& music);
    void stopMusic(const LuaMusic& music);
}