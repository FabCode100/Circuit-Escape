#include "DialogueSystem.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>

DialogueSystem::DialogueSystem()
    : font(nullptr), fontSmall(nullptr), active(false),
      currentMessageIndex(0), charIndex(0), charTimer(0),
      charDelay(0.04f), textComplete(false),
      boxAlpha(0), boxAnimTimer(0), 
      portraitTexture(nullptr), showPortrait(false) {
}

DialogueSystem::~DialogueSystem() {
    Shutdown();
}

bool DialogueSystem::Initialize(SDL_Renderer* renderer) {
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << "\n";
            return false;
        }
    }

    font = TTF_OpenFont("assets/PressStart2P-Regular.ttf", 10);
    if (!font) {
        std::cerr << "Warning: Could not load dialogue font: " << TTF_GetError() << "\n";
    }

    fontSmall = TTF_OpenFont("assets/PressStart2P-Regular.ttf", 8);
    if (!fontSmall) {
        std::cerr << "Warning: Could not load small font: " << TTF_GetError() << "\n";
    }

    SDL_Surface* portraitSurf = IMG_Load("assets/prof_leo.png");
    if (portraitSurf) {
        portraitTexture = SDL_CreateTextureFromSurface(renderer, portraitSurf);
        SDL_FreeSurface(portraitSurf);
    } else {
        std::cerr << "Warning: Could not load assets/prof_leo.png\n";
    }

    return true;
}

void DialogueSystem::ShowMessage(const std::string& speaker, const std::string& message, bool showPortrait) {
    messageQueue.clear();
    messageQueue.push_back(message);
    currentSpeaker = speaker;
    currentMessageIndex = 0;
    fullText = message;
    displayedText = "";
    charIndex = 0;
    charTimer = 0;
    textComplete = false;
    active = true;
    boxAlpha = 0;
    boxAnimTimer = 0;
    this->showPortrait = showPortrait;
}

void DialogueSystem::ShowMessages(const std::string& speaker, const std::vector<std::string>& messages, bool showPortrait) {
    if (messages.empty()) return;

    messageQueue = messages;
    currentSpeaker = speaker;
    currentMessageIndex = 0;
    fullText = messages[0];
    displayedText = "";
    charIndex = 0;
    charTimer = 0;
    textComplete = false;
    active = true;
    boxAlpha = 0;
    boxAnimTimer = 0;
    this->showPortrait = showPortrait;
}

void DialogueSystem::Update(float deltaTime) {
    if (!active) return;

    // Animação da caixa aparecendo
    if (boxAlpha < 220) {
        boxAlpha += 600 * deltaTime;
        if (boxAlpha > 220) boxAlpha = 220;
    }

    boxAnimTimer += deltaTime;

    // Efeito typewriter
    if (!textComplete) {
        charTimer += deltaTime;
        while (charTimer >= charDelay && charIndex < (int)fullText.size()) {
            charTimer -= charDelay;
            charIndex++;
            displayedText = fullText.substr(0, charIndex);
        }
        if (charIndex >= (int)fullText.size()) {
            textComplete = true;
        }
    }
}

void DialogueSystem::Render(SDL_Renderer* renderer, int screenW, int screenH) {
    if (!active) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Caixa de diálogo na parte inferior
    int boxH = 100;
    int boxW = screenW - 40;
    int boxX = 20;
    int boxY = screenH - boxH - 20;

    // Fundo da caixa
    SDL_SetRenderDrawColor(renderer, 10, 10, 40, (Uint8)boxAlpha);
    SDL_Rect bg = {boxX, boxY, boxW, boxH};
    SDL_RenderFillRect(renderer, &bg);

    // Retrato do Professor Leo
    if (showPortrait && portraitTexture) {
        SDL_Rect portraitRect = {boxX + 10, boxY + 10, 80, 80};
        SDL_RenderCopy(renderer, portraitTexture, nullptr, &portraitRect);
        
        // Borda do retrato
        SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
        SDL_RenderDrawRect(renderer, &portraitRect);
    }

    // Borda neon ciano
    SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
    SDL_RenderDrawRect(renderer, &bg);
    SDL_Rect bg2 = {boxX + 1, boxY + 1, boxW - 2, boxH - 2};
    SDL_RenderDrawRect(renderer, &bg2);

    // Pequena decoração nos cantos
    int cornerSize = 6;
    SDL_SetRenderDrawColor(renderer, 0, 255, 200, 255);
    // Canto superior esquerdo
    SDL_RenderDrawLine(renderer, boxX, boxY, boxX + cornerSize, boxY);
    SDL_RenderDrawLine(renderer, boxX, boxY, boxX, boxY + cornerSize);
    // Canto superior direito
    SDL_RenderDrawLine(renderer, boxX + boxW - 1, boxY, boxX + boxW - cornerSize - 1, boxY);
    SDL_RenderDrawLine(renderer, boxX + boxW - 1, boxY, boxX + boxW - 1, boxY + cornerSize);
    // Canto inferior esquerdo
    SDL_RenderDrawLine(renderer, boxX, boxY + boxH - 1, boxX + cornerSize, boxY + boxH - 1);
    SDL_RenderDrawLine(renderer, boxX, boxY + boxH - 1, boxX, boxY + boxH - cornerSize - 1);
    // Canto inferior direito
    SDL_RenderDrawLine(renderer, boxX + boxW - 1, boxY + boxH - 1, boxX + boxW - cornerSize - 1, boxY + boxH - 1);
    SDL_RenderDrawLine(renderer, boxX + boxW - 1, boxY + boxH - 1, boxX + boxW - 1, boxY + boxH - cornerSize - 1);

    if (!font) return;

    // Nome do speaker
    if (!currentSpeaker.empty()) {
        SDL_Color speakerColor = {0, 255, 200, 255};
        SDL_Surface* surf = TTF_RenderText_Solid(font, currentSpeaker.c_str(), speakerColor);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);

            // Background do nome
            SDL_SetRenderDrawColor(renderer, 10, 10, 40, 240);
            SDL_Rect nameBg = {boxX + 10, boxY - 14, surf->w + 12, surf->h + 6};
            SDL_RenderFillRect(renderer, &nameBg);
            SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
            SDL_RenderDrawRect(renderer, &nameBg);

            SDL_Rect dst = {boxX + 16, boxY - 11, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }

    // Texto com typewriter
    if (!displayedText.empty() && fontSmall) {
        SDL_Color textColor = {220, 220, 220, 255};
        // Quebra texto em linhas se necessário (wrapping simples)
        int textX = boxX + (showPortrait ? 100 : 15);
        int maxWidth = boxW - (showPortrait ? 115 : 30);
        SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(fontSmall, displayedText.c_str(), textColor, maxWidth);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dst = {textX, boxY + 15, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }

    // Indicador "pressione SPACE" quando texto completo
    if (textComplete && fontSmall) {
        float blink = std::sin(boxAnimTimer * 4.0f);
        if (blink > 0) {
            SDL_Color gray = {150, 150, 150, 255};
            const char* prompt = currentMessageIndex < (int)messageQueue.size() - 1
                                 ? ">> SPACE >>" : "[SPACE]";
            SDL_Surface* surf = TTF_RenderText_Solid(fontSmall, prompt, gray);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect dst = {boxX + boxW - surf->w - 15, boxY + boxH - surf->h - 10, surf->w, surf->h};
                SDL_RenderCopy(renderer, tex, nullptr, &dst);
                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surf);
            }
        }
    }
}

void DialogueSystem::Advance() {
    if (!active) return;

    if (!textComplete) {
        // Completa o texto imediatamente
        displayedText = fullText;
        charIndex = (int)fullText.size();
        textComplete = true;
        return;
    }

    // Avança para próxima mensagem
    currentMessageIndex++;
    if (currentMessageIndex >= (int)messageQueue.size()) {
        // Acabaram as mensagens
        active = false;
        return;
    }

    fullText = messageQueue[currentMessageIndex];
    displayedText = "";
    charIndex = 0;
    charTimer = 0;
    textComplete = false;
}

bool DialogueSystem::IsActive() const { return active; }

bool DialogueSystem::IsFinished() const {
    return !active && currentMessageIndex >= (int)messageQueue.size();
}

void DialogueSystem::Shutdown() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (fontSmall) {
        TTF_CloseFont(fontSmall);
        fontSmall = nullptr;
    }
    if (portraitTexture) {
        SDL_DestroyTexture(portraitTexture);
        portraitTexture = nullptr;
    }
}
