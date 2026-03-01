#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <SDL2/SDL_mixer.h>
#include <string>

class AudioSystem {
public:
    AudioSystem();
    ~AudioSystem();

    bool Initialize();
    void Shutdown();

    // Sons da Fase 1
    void PlaySuccess();
    void PlayError();
    void PlayExplosion();

    // Controle de volume
    void SetVolume(int volume); // 0-128

private:
    Mix_Chunk* successSound;
    Mix_Chunk* errorSound;
    Mix_Chunk* explosionSound;
    bool initialized;
};

#endif // AUDIO_SYSTEM_H
