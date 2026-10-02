#include "math.hpp"

#include <cmath>

namespace Math {
    float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    float clamp(float x, float low, float high) {
        return x < low ? low : (x > high ? high : x);
    }

    float step(float x, float step) {
        return std::floor(x / step) * step;
    }

    int sign(float x) {
        return (x > 0.0f) - (x < 0.0f);
    }
}