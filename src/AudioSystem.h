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

    // Música de Fundo
    enum class BGMType { MENU, GAME, PUZZLE };
    void PlayBGM(BGMType type);
    void StopBGM();

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
    Mix_Music* menuBGM;
    Mix_Music* gameBGM;
    Mix_Music* puzzleBGM;
    bool initialized;
};

#endif // AUDIO_SYSTEM_H
