#ifndef DIALOGUE_SYSTEM_H
#define DIALOGUE_SYSTEM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

class DialogueSystem {
public:
    DialogueSystem();
    ~DialogueSystem();

    bool Initialize(SDL_Renderer* renderer);
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer, int screenW, int screenH);
    void Shutdown();

    // Mostra uma mensagem com efeito typewriter e retrato
    void ShowMessage(const std::string& speaker, const std::string& message, bool showPortrait = true);
    void ShowMessages(const std::string& speaker, const std::vector<std::string>& messages, bool showPortrait = true);

    // Controle
    void Advance(); // Avança para próxima mensagem ou completa texto
    bool IsActive() const;
    bool IsFinished() const;

private:
    TTF_Font* font;
    TTF_Font* fontSmall;

    // Estado do diálogo
    bool active;
    std::string currentSpeaker;
    std::vector<std::string> messageQueue;
    int currentMessageIndex;

    // Efeito typewriter
    std::string displayedText;
    std::string fullText;
    int charIndex;
    float charTimer;
    float charDelay; // Segundos entre cada caractere
    bool textComplete;

    // Visual
    float boxAlpha;
    float boxAnimTimer;
    SDL_Texture* portraitTexture;
    bool showPortrait;
};

#endif // DIALOGUE_SYSTEM_H
