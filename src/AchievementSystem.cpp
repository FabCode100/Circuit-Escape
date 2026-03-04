#include "AchievementSystem.h"
#include <iostream>

AchievementSystem::AchievementSystem() 
    : isDisplaying(false), titleFont(nullptr), descFont(nullptr) {}

AchievementSystem::~AchievementSystem() {
    Shutdown();
}

bool AchievementSystem::Initialize(SDL_Renderer* renderer) {
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            std::cerr << "Failed to init TTF in AchievementSystem\n";
            return false;
        }
    }
    titleFont = TTF_OpenFont("assets/PressStart2P-Regular.ttf", 10);
    descFont = TTF_OpenFont("assets/PressStart2P-Regular.ttf", 8);
    if (!titleFont || !descFont) {
        std::cerr << "Warning: Could not load Achievement fonts\n";
    }
    return true;
}

void AchievementSystem::Shutdown() {
    if (titleFont) { TTF_CloseFont(titleFont); titleFont = nullptr; }
    if (descFont) { TTF_CloseFont(descFont); descFont = nullptr; }
}

bool AchievementSystem::Unlock(const std::string& id, const std::string& title, const std::string& description) {
    if (unlockedIds.find(id) != unlockedIds.end()) {
        return false; // Conquista já existe, ignora
    }
    unlockedIds.insert(id);
    
    Achievement ach;
    ach.title = title;
    ach.description = description;
    ach.timer = 5.0f; // 5 segundos na tela
    ach.alpha = 0.0f;
    ach.yOffset = -100.0f; // Começa escondido (acima)
    
    queue.push_back(ach);
    std::cout << "[ACHIEVEMENT UNLOCKED] " << title << "\n";
    return true;
}

void AchievementSystem::Update(float deltaTime) {
    if (queue.empty()) {
        isDisplaying = false;
        return;
    }

    isDisplaying = true;
    Achievement& current = queue.front();

    // Animando slide in/out e alpha
    if (current.timer > 4.5f) { // Aparecendo na tela
        current.yOffset += 400.0f * deltaTime;
        if (current.yOffset > 20.0f) current.yOffset = 20.0f;
        current.alpha += 500.0f * deltaTime;
        if (current.alpha > 255.0f) current.alpha = 255.0f;
    } else if (current.timer < 0.5f) { // Sumindo
        current.yOffset -= 300.0f * deltaTime;
        current.alpha -= 500.0f * deltaTime;
        if (current.alpha < 0.0f) current.alpha = 0.0f;
    } else {
        current.yOffset = 20.0f;
        current.alpha = 255.0f;
    }

    current.timer -= deltaTime;
    if (current.timer <= 0.0f) {
        queue.erase(queue.begin());
    }
}

void AchievementSystem::Render(SDL_Renderer* renderer, int screenW, int screenH) {
    if (!isDisplaying || queue.empty() || !titleFont || !descFont) return;

    Achievement& current = queue.front();
    
    int boxW = 400;
    if (boxW > screenW - 40) boxW = screenW - 40;
    int boxH = 65;
    int boxX = (screenW - boxW) / 2;
    int boxY = (int)current.yOffset;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Fundo
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, (Uint8)current.alpha);
    SDL_Rect bg = {boxX, boxY, boxW, boxH};
    SDL_RenderFillRect(renderer, &bg);

    // Borda Dourada / Brilhante
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, (Uint8)current.alpha);
    SDL_RenderDrawRect(renderer, &bg);
    SDL_Rect bg2 = {boxX + 1, boxY + 1, boxW - 2, boxH - 2};
    SDL_RenderDrawRect(renderer, &bg2);

    // Ícone estilo Troféu Minimalista
    SDL_Rect iconRect = {boxX + 10, boxY + 12, 40, 40};
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, (Uint8)current.alpha);
    SDL_RenderFillRect(renderer, &iconRect);
    
    // Detalhe interno do Troféu
    SDL_SetRenderDrawColor(renderer, 150, 100, 0, (Uint8)current.alpha);
    SDL_Rect innerTrophy = {boxX + 15, boxY + 17, 30, 15};
    SDL_RenderFillRect(renderer, &innerTrophy);
    SDL_RenderDrawLine(renderer, boxX + 30, boxY + 32, boxX + 30, boxY + 45); // Haste
    SDL_RenderDrawLine(renderer, boxX + 20, boxY + 45, boxX + 40, boxY + 45); // Base

    // Título da Conquista
    SDL_Color gold = {255, 215, 0, (Uint8)current.alpha};
    SDL_Surface* titleSurf = TTF_RenderText_Solid(titleFont, current.title.c_str(), gold);
    if (titleSurf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, titleSurf);
        SDL_Rect textRect = {boxX + 60, boxY + 12, titleSurf->w, titleSurf->h};
        SDL_RenderCopy(renderer, tex, nullptr, &textRect);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(titleSurf);
    }

    // Descrição da Conquista
    SDL_Color white = {200, 200, 200, (Uint8)current.alpha};
    SDL_Surface* descSurf = TTF_RenderText_Blended_Wrapped(descFont, current.description.c_str(), white, boxW - 70);
    if (descSurf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, descSurf);
        SDL_Rect textRect = {boxX + 60, boxY + 30, descSurf->w, descSurf->h};
        SDL_RenderCopy(renderer, tex, nullptr, &textRect);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(descSurf);
    }
}
