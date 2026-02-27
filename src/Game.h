#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Player.h"
#include "CircuitSystem.h"
#include "DialogueSystem.h"

const int SCALE = 2;
const int SCREEN_WIDTH = 128 * SCALE;
const int SCREEN_HEIGHT = 160 * SCALE;
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

class Game {
public:
    Game();
    ~Game();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    void HandleInput();
    void Update();
    void Render();

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    
    Player player;
    CircuitSystem circuitSystem;
    DialogueSystem dialogueSystem;
};

#endif // GAME_H
