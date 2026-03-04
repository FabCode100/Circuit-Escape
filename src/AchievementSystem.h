#ifndef ACHIEVEMENT_SYSTEM_H
#define ACHIEVEMENT_SYSTEM_H

#include <string>
#include <vector>
#include <set>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class AchievementSystem {
public:
    AchievementSystem();
    ~AchievementSystem();

    bool Initialize(SDL_Renderer* renderer);
    void Shutdown();

    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer, int screenW, int screenH);

    // Retorna true se a conquista foi destravada neste momento
    bool Unlock(const std::string& id, const std::string& title, const std::string& description);

private:
    struct Achievement {
        std::string title;
        std::string description;
        float timer; // Tempo restante na tela
        float alpha; // Para fade in/out
        float yOffset; // Posição Y para animação
    };

    std::set<std::string> unlockedIds;
    std::vector<Achievement> queue;
    bool isDisplaying;

    TTF_Font* titleFont;
    TTF_Font* descFont;
};

#endif // ACHIEVEMENT_SYSTEM_H
