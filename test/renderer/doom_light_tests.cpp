// SPDX-License-Identifier: GPL-3.0-or-later
#include "renderer/tr_doomlighting.h"
#include <cstdio>
int main() {
    using namespace doomlight;
    if (FogIndex(-1) != 1 || FogIndex(999) != 16) {
        return 1;
    }
    for (int light = -5; light < 260; ++light) {
        for (int depth = 0; depth < 4096; depth += 32) {
            const int alpha = Alpha(light, static_cast<float>(depth));
            // depth must not matter; only the sector light darkens the surface
            const int level = light < 0 ? 0 : light > 255 ? 255 : light;
            if (alpha != 255 - level) {
                return 2;
            }
        }
    }
    std::puts("Doom sector light shading, fullbright and monotonic visibility passed.");
}
