#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Player.h"
#include "CircuitSystem.h"
#include "DialogueSystem.h"
#include "TileMap.h"
#include "Camera.h"

const int SCALE = 3;
const int SCREEN_WIDTH = 192 * SCALE;
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
    
    SDL_Texture* tilesetTexture;
    SDL_Texture* objectsTexture;
    
    Player player;
    CircuitSystem circuitSystem;
    DialogueSystem dialogueSystem;
    TileMap* tileMap;
    Camera* camera;
};

#endif // GAME_H
