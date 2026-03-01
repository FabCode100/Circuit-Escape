#include "AudioSystem.h"
#include <iostream>

AudioSystem::AudioSystem()
    : successSound(nullptr), errorSound(nullptr), explosionSound(nullptr),
      menuBGM(nullptr), gameBGM(nullptr), puzzleBGM(nullptr), initialized(false) {
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

    menuBGM = Mix_LoadMUS("assets/sounds/menu_music.mp3");
    if (!menuBGM) {
        std::cerr << "Warning: Could not load menu_music.mp3: " << Mix_GetError() << "\n";
    }

    gameBGM = Mix_LoadMUS("assets/sounds/game_music.mp3");
    if (!gameBGM) {
        std::cerr << "Warning: Could not load game_music.mp3: " << Mix_GetError() << "\n";
    }

    puzzleBGM = Mix_LoadMUS("assets/sounds/puzzle_music.mp3");
    if (!puzzleBGM) {
        std::cerr << "Warning: Could not load puzzle_music.mp3: " << Mix_GetError() << "\n";
    }

    return true;
}

void AudioSystem::Shutdown() {
    if (menuBGM) { Mix_FreeMusic(menuBGM); menuBGM = nullptr; }
    if (gameBGM) { Mix_FreeMusic(gameBGM); gameBGM = nullptr; }
    if (puzzleBGM) { Mix_FreeMusic(puzzleBGM); puzzleBGM = nullptr; }
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

void AudioSystem::PlayBGM(BGMType type) {
    Mix_Music* target = nullptr;
    switch (type) {
        case BGMType::MENU: target = menuBGM; break;
        case BGMType::GAME: target = gameBGM; break;
        case BGMType::PUZZLE: target = puzzleBGM; break;
    }

    if (target) {
        if (Mix_PlayingMusic()) {
            Mix_FadeOutMusic(500);
            Mix_FadeInMusic(target, -1, 500);
        } else {
            Mix_PlayMusic(target, -1);
        }
    }
}

void AudioSystem::StopBGM() {
    Mix_HaltMusic();
}
