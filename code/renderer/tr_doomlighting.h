// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

namespace doomlight {
constexpr int Levels = 16;
constexpr int Width = 1024;
constexpr float DistanceStep = 4.0f;
inline int FogIndex(int light) {
    return 1 + (light < 0 ? 0 : light > 255 ? 255 : light) / 16;
}
// Surfaces are darkened by their sector light level only. Depth is ignored so
// the player no longer acts as a light source standing near walls.
inline unsigned char Alpha(int light, float depth) {
    (void)depth;
    const int level = light < 0 ? 0 : light > 255 ? 255 : light;
    return static_cast<unsigned char>(255 - level);
}
} // namespace doomlight
