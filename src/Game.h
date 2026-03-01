#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "GameState.h"
#include "Player.h"
#include "CircuitSystem.h"
#include "CircuitPuzzle.h"
#include "DialogueSystem.h"
#include "TileMap.h"
#include "Camera.h"
#include "VFX.h"
#include "AudioSystem.h"
#include "HardwareInterface.h"

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
    // Dispatch por estado
    void HandleInput();
    void Update();
    void Render();

    // HandleInput por estado
    void HandleInput_Exploration();
    void HandleInput_Puzzle();
    void HandleInput_Dialogue();

    // Update por estado
    void Update_Exploration();
    void Update_Puzzle(float deltaTime);
    void Update_Dialogue(float deltaTime);
    void Update_Victory(float deltaTime);

    // Render por estado
    void Render_Exploration();
    void Render_Puzzle();
    void Render_Dialogue();
    void Render_Victory();

    // Transições de estado
    void EnterPuzzle(int puzzleId);
    void ExitPuzzle();
    void EnterDialogue(const std::string& speaker, const std::vector<std::string>& messages);
    void ExitDialogue();
    void EnterVictory();

    // Processa resultado do puzzle
    void HandlePuzzleResult(PuzzleResult result);

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    // Estado atual
    GameState currentState;

    // Texturas
    SDL_Texture* tilesetTexture;
    SDL_Texture* objectsTexture;

    // Subsistemas
    Player player;
    CircuitSystem circuitSystem;
    CircuitPuzzle circuitPuzzle;
    DialogueSystem dialogueSystem;
    VFX vfx;
    AudioSystem audioSystem;
    HardwareInterface hardwareInterface;
    TileMap* tileMap;
    Camera* camera;

    // Timing
    Uint32 lastFrameTime;

    // Puzzle trigger zone (posição no mapa que abre o puzzle)
    int puzzleTriggerX;
    int puzzleTriggerY;
    bool puzzleSolved;
    PuzzleResult lastPuzzleResult;
    int pendingPuzzleId;
    bool firstTimePuzzle;
};

#endif // GAME_H
