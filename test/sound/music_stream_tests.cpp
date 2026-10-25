// SPDX-License-Identifier: GPL-3.0-or-later
#define AL_ALEXT_PROTOTYPES
#include "sound/snd_local.h"
#include "third-party/openal/include/AL/al.h"
#include "third-party/openal/include/AL/alc.h"
#include "third-party/openal/include/AL/alext.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

// snd_local.h pulls in client.h, which has no include guard, so it must
// not be included twice.

namespace {
std::string report;
}

// The music module and the port's stream helpers go through the engine C
// API; the test provides everything they reference.
void QDECL Com_Printf(const char *format, ...) {
    char text[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);
    report = text;
}
void QDECL Com_Error(int, const char *message, ...) {
    throw std::runtime_error(message);
}
char *Cvar_VariableString(const char *) {
    static char root[] = ".";
    return root;
}
int FS_ReadFile(const char *name, void **buffer) {
    std::ifstream input(std::string("base/") + name, std::ios::binary | std::ios::ate);
    if (!input.good()) {
        *buffer = nullptr;
        return -1;
    }
    const int length = static_cast<int>(input.tellg());
    input.seekg(0);
    *buffer = malloc(length);
    input.read(static_cast<char *>(*buffer), length);
    return length;
}
void FS_FreeFile(void *buffer) {
    free(buffer);
}
// snd_hardware.cpp locks the sound system through this pointer.
idSoundSystemLocal *soundSystemLocalPtr = nullptr;

void Check(bool condition, const char *why) {
    if (!condition)
        throw std::runtime_error(why);
}

int main() {
    try {
        auto *device = alcLoopbackOpenDeviceSOFT(nullptr);
        Check(device, "OpenAL loopback device");
        const ALCint attributes[] = {
            ALC_FREQUENCY,  44100, ALC_FORMAT_CHANNELS_SOFT, ALC_STEREO_SOFT, ALC_FORMAT_TYPE_SOFT,
            ALC_SHORT_SOFT, 0};
        auto *context = alcCreateContext(device, attributes);
        Check(context && alcMakeContextCurrent(context), "OpenAL context");
        MusicInit();
        std::vector<int16_t> output(4096 * 2);
        long long energy = 0;
        for (int i = 0; i < 250; ++i) {
            // The UI may request the same track repeatedly; its timeline must advance.
            MusicStart("music/doom/d_intro.mid", "music/doom/d_intro.mid");
            MusicUpdatePump(0.5f);
            if (MusicLoadPending()) {
                MusicLoadQueued(0.5f);
            }
            alcRenderSamplesSOFT(device, output.data(), 4096);
            for (auto sample : output)
                energy += abs(int(sample));
        }
        MusicInfo();
        int playing, queued, loops;
        unsigned position;
        Check(sscanf(report.c_str(),
                     "Music: track=%*s playing=%d queued=%d position=%u loops=%d", &playing,
                     &queued, &position, &loops) == 4 &&
                  playing && queued > 0 && loops > 0 && energy > 0,
              "Real music streaming must remain audible through title-track loops and repeated "
              "requests");
        std::vector<int16_t> stall(44100 * 2);
        alcRenderSamplesSOFT(device, stall.data(), 44100);
        MusicUpdatePump(0.5f);
        if (MusicLoadPending()) {
            MusicLoadQueued(0.5f);
        }
        MusicInfo();
        Check(report.find("playing=1") != std::string::npos && alGetError() == AL_NO_ERROR,
              "Music recovers after queue starvation");
        MusicStop();
        MusicInfo();
        Check(report.find("playing=0") != std::string::npos, "Explicit music stop");
        MusicStart("music/doom/d_e1m2.mid", "music/doom/d_e1m2.mid");
        MusicUpdatePump(0.5f);
        alcRenderSamplesSOFT(device, output.data(), 4096);
        MusicInfo();
        Check(report.find("playing=1") != std::string::npos, "Destination music starts after stop");
        // WAV tracks go through the same source as the MIDI scores.
        MusicStop();
        MusicStart("sound/doom/weapons/pistol.wav", "sound/doom/weapons/pistol.wav");
        MusicUpdatePump(0.5f);
        alcRenderSamplesSOFT(device, output.data(), 4096);
        MusicInfo();
        Check(report.find("playing=1") != std::string::npos &&
                  report.find("backend=pcm") != std::string::npos,
              "WAV PCM fallback plays through the music module");
        MusicShutdown();
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);
        std::puts("Music module passed: sustained playback, repeated requests, complete loops, "
                  "starvation recovery, stop, track replacement and WAV fallback.");
    } catch (const std::exception &error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
