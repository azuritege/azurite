#pragma once

namespace Math {
    float lerp(float a, float b, float t);
    float clamp(float x, float low, float high);
    float step(float x, float step);
    int sign(float x);
}