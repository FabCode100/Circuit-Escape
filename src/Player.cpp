#include "Player.h"
#include "Game.h"
#include <iostream>
#include <SDL2/SDL_image.h>
#include <chrono>

Player::Player() : gridX(3), gridY(3), isMoving(false), direction(DOWN), frame(0), frameCounter(0), texture(nullptr), lastMoveTime(std::chrono::steady_clock::now()) {
}

Player::~Player() {
    Shutdown();
}

bool Player::Initialize(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/player.png");
    if (!surface) {
        std::cerr << "Error loading player sprite: " << IMG_GetError() << "\n";
        return false;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Error creating texture: " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

void Player::Shutdown() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void Player::SetTexture(SDL_Texture* tex) {
    texture = tex;
}

void Player::MoveUp() {
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime).count() < moveDelayMs) return;
    gridY--;
    direction = UP;
    isMoving = true;
    lastMoveTime = now;
}

void Player::MoveDown() {
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime).count() < moveDelayMs) return;
    gridY++;
    direction = DOWN;
    isMoving = true;
    lastMoveTime = now;
}

void Player::MoveLeft() {
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime).count() < moveDelayMs) return;
    gridX--;
    direction = LEFT;
    isMoving = true;
    lastMoveTime = now;
}

void Player::MoveRight() {
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime).count() < moveDelayMs) return;
    gridX++;
    direction = RIGHT;
    isMoving = true;
    lastMoveTime = now;
}

void Player::SetMoving(bool moving) {
    isMoving = moving;
}

bool Player::IsMoving() const {
    return isMoving;
}

void Player::Update() {
    // Animation logic
    if (isMoving) {
        frameCounter++;
        if (frameCounter >= 10) {
            frame = (frame + 1) % FRAMES_PER_DIRECTION;
            frameCounter = 0;
        }
    } else {
        frame = 1; // Idle frame (middle)
    }
}

void Player::Render(SDL_Renderer* renderer) {
    if (!texture) return;

    SDL_Rect srcRect = {
        frame * FRAME_WIDTH,
        (int)direction * FRAME_HEIGHT,
        FRAME_WIDTH,
        FRAME_HEIGHT
    };

    SDL_Rect destRect = {
        gridX * TILE_SIZE * SCALE,
        gridY * TILE_SIZE * SCALE,
        FRAME_WIDTH * SCALE,
        FRAME_HEIGHT * SCALE
    };

    // render with nearest-neighbor scaling if necessary
    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
}

int Player::GetGridX() const {
    return gridX;
}

int Player::GetGridY() const {
    return gridY;
}
