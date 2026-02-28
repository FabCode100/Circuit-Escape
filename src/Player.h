#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <chrono>

class TileMap;
class CircuitSystem;

const int TILE_SIZE = 32;
// sprite sheet is now 128x128: 4 columns (frames) × 4 rows (directions)
const int FRAME_WIDTH = 32;    // 128 / 4
const int FRAME_HEIGHT = 32;   // 128 / 4
const int FRAMES_PER_DIRECTION = 4;

enum Direction { DOWN = 0, LEFT = 1, RIGHT = 2, UP = 3 };

class Player {
public:
    Player();
    ~Player();

    bool Initialize(SDL_Renderer* renderer);
    void Shutdown();
    void SetTexture(SDL_Texture* tex);
    void SetTileMap(TileMap* map);
    void SetCircuitSystem(CircuitSystem* cs);

    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();
    void SetMoving(bool moving);
    bool IsMoving() const;

    void Update();
    void Render(SDL_Renderer* renderer, int camX, int camY);

    int GetGridX() const;
    int GetGridY() const;
    float GetWorldX() const;
    float GetWorldY() const;

private:
    int gridX;
    int gridY;
    float worldX;
    float worldY;
    bool isMoving;
    Direction direction;
    int frame;
    int frameCounter;
    SDL_Texture* texture;
    TileMap* tileMap;
    CircuitSystem* circuitSystem;
    std::chrono::steady_clock::time_point lastMoveTime;
    int moveDelayMs = 220;
};

#endif // PLAYER_H
