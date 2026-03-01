#include "AudioSystem.h"
#include <iostream>

AudioSystem::AudioSystem()
    : successSound(nullptr), errorSound(nullptr), explosionSound(nullptr),
      initialized(false) {
}

AudioSystem::~AudioSystem() {
    Shutdown();
}

bool AudioSystem::Initialize() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Warning: Could not initialize SDL_mixer: " << Mix_GetError() << "\n";
        std::cerr << "Audio will be disabled.\n";
        return false; // Não é fatal - jogo funciona sem áudio
    }

    initialized = true;

    // Tenta carregar sons (não é fatal se falhar)
    successSound = Mix_LoadWAV("assets/sounds/success.wav");
    if (!successSound) {
        std::cerr << "Warning: Could not load success.wav: " << Mix_GetError() << "\n";
    }

    errorSound = Mix_LoadWAV("assets/sounds/error.wav");
    if (!errorSound) {
        std::cerr << "Warning: Could not load error.wav: " << Mix_GetError() << "\n";
    }

    explosionSound = Mix_LoadWAV("assets/sounds/explosion.wav");
    if (!explosionSound) {
        std::cerr << "Warning: Could not load explosion.wav: " << Mix_GetError() << "\n";
    }

    return true;
}

void AudioSystem::Shutdown() {
    if (successSound) { Mix_FreeChunk(successSound); successSound = nullptr; }
    if (errorSound) { Mix_FreeChunk(errorSound); errorSound = nullptr; }
    if (explosionSound) { Mix_FreeChunk(explosionSound); explosionSound = nullptr; }

    if (initialized) {
        Mix_CloseAudio();
        initialized = false;
    }
}

void AudioSystem::PlaySuccess() {
    if (successSound) {
        Mix_PlayChannel(-1, successSound, 0);
    } else {
        std::cout << "[AUDIO] Success sound\n";
    }
}

void AudioSystem::PlayError() {
    if (errorSound) {
        Mix_PlayChannel(-1, errorSound, 0);
    } else {
        std::cout << "[AUDIO] Error sound\n";
    }
}

void AudioSystem::PlayExplosion() {
    if (explosionSound) {
        Mix_PlayChannel(-1, explosionSound, 0);
    } else {
        std::cout << "[AUDIO] Explosion sound\n";
    }
}

void AudioSystem::SetVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    Mix_Volume(-1, volume);
}
