#ifndef CIRCUIT_SYSTEM_H
#define CIRCUIT_SYSTEM_H

#include <SDL2/SDL.h>
#include <vector>

enum class CircuitType {
    SWITCH,
    DOOR
};

struct CircuitElement {
    int id; // Link do switch para a porta (ex: Switch 1 liga Door 1)
    CircuitType type;
    int gridX;
    int gridY;
    bool isActive; // true = ON ou Aberta
};

class CircuitSystem {
public:
    CircuitSystem();
    ~CircuitSystem();

    void Initialize(SDL_Texture* objTexture);
    void AddElement(CircuitType type, int id, int x, int y);
    void Update();
    void Render(SDL_Renderer* renderer, int camX, int camY, int scale);
    void Shutdown();

    void Interact(int playerX, int playerY);
    bool IsSolid(int x, int y) const;

private:
    std::vector<CircuitElement> elements;
    SDL_Texture* objectTexture;
};

#endif // CIRCUIT_SYSTEM_H
