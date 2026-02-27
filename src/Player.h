#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>

const int TILE_SIZE = 32;
// sprite sheet is now 96x128: 3 columns (frames) × 4 rows (directions)
const int FRAME_WIDTH = 32;    // 96 / 3
const int FRAME_HEIGHT = 32;   // 128 / 4
const int FRAMES_PER_DIRECTION = 3;

enum Direction { DOWN = 0, LEFT = 1, RIGHT = 2, UP = 3 };

class Player {
public:
    Player();
    ~Player();

    bool Initialize(SDL_Renderer* renderer);
    void Shutdown();
    void SetTexture(SDL_Texture* tex);

    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();
    void SetMoving(bool moving);
    bool IsMoving() const;

    void Update();
    void Render(SDL_Renderer* renderer);

    int GetGridX() const;
    int GetGridY() const;

private:
    int gridX;
    int gridY;
    bool isMoving;
    Direction direction;
    int frame;
    int frameCounter;
    SDL_Texture* texture;
};

#endif // PLAYER_H
