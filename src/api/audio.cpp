#include "audio.hpp"

namespace Audio {
    LuaSound::LuaSound(const std::string& name) {
        sound = LoadSound(
            (std::string(GetApplicationDirectory()) + name).c_str()
        );
    }
    LuaSound::LuaSound(LuaSound&& other) noexcept
    : sound(std::exchange(other.sound, Sound{})) {}

    LuaSound& LuaSound::operator=(LuaSound&& other) noexcept {
        if (this != &other) {
            release();
            sound = std::exchange(other.sound, Sound{});
        }
        return *this;
    }

    LuaSound::~LuaSound() {
        release();
    }

    void LuaSound::release() {
        if (sound.stream.buffer != nullptr) {
            UnloadSound(sound);
            sound = {};
        }
    }

    LuaSound loadSound(const std::string& name) {
        return LuaSound{name};
    }

    void playSound(const LuaSound& sound) {
        PlaySound(sound.sound);
    }

    void stopSound(const LuaSound& sound) {
        StopSound(sound.sound);
    }

    LuaMusic::LuaMusic(const std::string& name) {
        music = LoadMusicStream(
            (std::string(GetApplicationDirectory()) + name).c_str()
        );
    }
    LuaMusic::LuaMusic(LuaMusic&& other) noexcept
    : music(std::exchange(other.music, Music{})) {}

    LuaMusic& LuaMusic::operator=(LuaMusic&& other) noexcept {
        if (this != &other) {
            release();
            music = std::exchange(other.music, Music{});
        }
        return *this;
    }

    LuaMusic::~LuaMusic() {
        release();
    }

    void LuaMusic::release() {
        if (music.stream.buffer != nullptr) {
            UnloadMusicStream(music);
            music = {};
        }
    }

    LuaMusic loadMusic(const std::string& name) {
        return LuaMusic{name};
    }

    void playMusic(const LuaMusic& music) {
        PlayMusicStream(music.music);
    }

    void updateMusic(const LuaMusic& music) {
        UpdateMusicStream(music.music);
    }

    void stopMusic(const LuaMusic& music) {
        StopMusicStream(music.music);
    }
}