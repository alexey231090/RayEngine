#include "SoundFX.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>

namespace REngine {

bool SoundFX::s_initialized = false;
Sound SoundFX::s_clickSound = { 0 };
Sound SoundFX::s_coinSound = { 0 };
Sound SoundFX::s_explosionSound = { 0 };
Sound SoundFX::s_fallSound = { 0 };
Sound SoundFX::s_lineClearSound = { 0 };

static const float PI_CONST = 3.14159265358979323846f;

Sound SoundFX::GenerateToneSound(float frequency, float durationSec, float volume) {
    int sampleRate = 44100;
    int totalSamples = (int)(sampleRate * durationSec);
    if (totalSamples <= 0) totalSamples = 1;

    short* data = (short*)MemAlloc(totalSamples * sizeof(short));

    for (int i = 0; i < totalSamples; ++i) {
        float t = (float)i / (float)sampleRate;
        float progress = (float)i / (float)totalSamples;

        // Exponential decay envelope to avoid clicks
        float envelope = (1.0f - progress) * (1.0f - progress);

        float sample = std::sin(2.0f * PI_CONST * frequency * t);
        data[i] = (short)(sample * volume * envelope * 32767.0f);
    }

    Wave wave = { 0 };
    wave.frameCount = totalSamples;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = data;

    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}

Sound SoundFX::GenerateNoiseSound(float durationSec, float volume) {
    int sampleRate = 44100;
    int totalSamples = (int)(sampleRate * durationSec);
    if (totalSamples <= 0) totalSamples = 1;

    short* data = (short*)MemAlloc(totalSamples * sizeof(short));

    for (int i = 0; i < totalSamples; ++i) {
        float progress = (float)i / (float)totalSamples;
        float envelope = (1.0f - progress) * (1.0f - progress);

        float randomVal = ((float)std::rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        data[i] = (short)(randomVal * volume * envelope * 32767.0f);
    }

    Wave wave = { 0 };
    wave.frameCount = totalSamples;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = data;

    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}

void SoundFX::Init() {
    if (s_initialized) return;

    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        std::cerr << "[REngine] [Audio] Warning: Audio device not available." << std::endl;
        return;
    }

    // 1. Click: fast 850 Hz click
    s_clickSound = GenerateToneSound(850.0f, 0.04f, 0.4f);

    // 2. Fall: low 130 Hz impact thud
    s_fallSound = GenerateToneSound(130.0f, 0.12f, 0.5f);

    // 3. Explosion: noisy burst
    s_explosionSound = GenerateNoiseSound(0.35f, 0.6f);

    // 4. Coin: pleasant two-tone sound
    {
        int sampleRate = 44100;
        int totalSamples = (int)(sampleRate * 0.2f);
        short* data = (short*)MemAlloc(totalSamples * sizeof(short));
        int mid = totalSamples / 2;

        for (int i = 0; i < totalSamples; ++i) {
            float t = (float)i / (float)sampleRate;
            float freq = (i < mid) ? 987.0f : 1318.0f;
            float progress = (float)i / (float)totalSamples;
            float envelope = 1.0f - progress;
            float sample = std::sin(2.0f * PI_CONST * freq * t);
            data[i] = (short)(sample * 0.4f * envelope * 32767.0f);
        }

        Wave wave = { 0 };
        wave.frameCount = totalSamples;
        wave.sampleRate = sampleRate;
        wave.sampleSize = 16;
        wave.channels = 1;
        wave.data = data;
        s_coinSound = LoadSoundFromWave(wave);
        UnloadWave(wave);
    }

    // 5. Line Clear: triumphant chord progression
    {
        int sampleRate = 44100;
        int totalSamples = (int)(sampleRate * 0.35f);
        short* data = (short*)MemAlloc(totalSamples * sizeof(short));
        int part = totalSamples / 3;

        for (int i = 0; i < totalSamples; ++i) {
            float t = (float)i / (float)sampleRate;
            float freq = 523.0f;
            if (i >= part && i < part * 2) freq = 659.0f;
            else if (i >= part * 2) freq = 784.0f;

            float progress = (float)i / (float)totalSamples;
            float envelope = 1.0f - (progress * 0.7f);
            float sample = std::sin(2.0f * PI_CONST * freq * t);
            data[i] = (short)(sample * 0.45f * envelope * 32767.0f);
        }

        Wave wave = { 0 };
        wave.frameCount = totalSamples;
        wave.sampleRate = sampleRate;
        wave.sampleSize = 16;
        wave.channels = 1;
        wave.data = data;
        s_lineClearSound = LoadSoundFromWave(wave);
        UnloadWave(wave);
    }

    s_initialized = true;
    std::cout << "[REngine] [Audio] SoundFX procedural audio initialized successfully." << std::endl;
}

void SoundFX::PlayClick() {
    if (s_initialized) PlaySound(s_clickSound);
}

void SoundFX::PlayCoin() {
    if (s_initialized) PlaySound(s_coinSound);
}

void SoundFX::PlayExplosion() {
    if (s_initialized) PlaySound(s_explosionSound);
}

void SoundFX::PlayFall() {
    if (s_initialized) PlaySound(s_fallSound);
}

void SoundFX::PlayLineClear() {
    if (s_initialized) PlaySound(s_lineClearSound);
}

void SoundFX::PlayTone(float frequency, float durationSec, float volume) {
    if (!s_initialized) return;
    Sound sound = GenerateToneSound(frequency, durationSec, volume);
    PlaySound(sound);
    // Raylib unloads finished sounds or we can reuse static channels
}

void SoundFX::Shutdown() {
    if (!s_initialized) return;

    UnloadSound(s_clickSound);
    UnloadSound(s_coinSound);
    UnloadSound(s_explosionSound);
    UnloadSound(s_fallSound);
    UnloadSound(s_lineClearSound);

    CloseAudioDevice();
    s_initialized = false;
    std::cout << "[REngine] [Audio] Audio device closed cleanly." << std::endl;
}

} // namespace REngine
