#pragma once

#include "raylib.h"
#include <vector>

namespace REngine {

class SoundFX {
public:
    static void Init();
    static void Shutdown();

    // Built-in synthesized procedural sounds
    static void PlayClick();
    static void PlayCoin();
    static void PlayExplosion();
    static void PlayFall();
    static void PlayLineClear();

    // Play custom tone on the fly
    static void PlayTone(float frequency, float durationSec = 0.1f, float volume = 0.4f);

private:
    static Sound GenerateToneSound(float frequency, float durationSec, float volume = 0.4f);
    static Sound GenerateNoiseSound(float durationSec, float volume = 0.4f);

private:
    static bool s_initialized;
    static Sound s_clickSound;
    static Sound s_coinSound;
    static Sound s_explosionSound;
    static Sound s_fallSound;
    static Sound s_lineClearSound;
};

} // namespace REngine
