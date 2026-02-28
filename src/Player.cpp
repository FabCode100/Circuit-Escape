#include "Player.h"
#include "Game.h"
#include <iostream>
#include <SDL2/SDL_image.h>
#include <chrono>
#include "TileMap.h"
#include "CircuitSystem.h"

Player::Player() : gridX(3), gridY(3), worldX(3 * TILE_SIZE), worldY(3 * TILE_SIZE), isMoving(false), direction(DOWN), frame(0), frameCounter(0), texture(nullptr), tileMap(nullptr), circuitSystem(nullptr), lastMoveTime(std::chrono::steady_clock::now()) {
}

Player::~Player() {
    Shutdown();
}

bool Player::Initialize(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/player_scifi.png");
    if (!surface) {
        std::cerr << "Error loading player sprite: " << IMG_GetError() << "\n";
        return false;
    }

    // Define a cor branca do fundo como transparente (Color Key)
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255));

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

void Player::SetTileMap(TileMap* map) {
    tileMap = map;
}

void Player::SetCircuitSystem(CircuitSystem* cs) {
    circuitSystem = cs;
}

void Player::MoveUp() {
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime).count() < moveDelayMs) return;
    direction = UP;
    lastMoveTime = now;
    isMoving = true; // Ativa animação
    if (tileMap && !tileMap->GetTile(gridX, gridY - 1).IsSolid()) {
        if (!circuitSystem || !circuitSystem->IsSolid(gridX, gridY - 1)) {
            gridY--;
        }
    }
}

void Player::MoveDown() {
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime).count() < moveDelayMs) return;
    direction = DOWN;
    lastMoveTime = now;
    isMoving = true; // Ativa animação
    if (tileMap && !tileMap->GetTile(gridX, gridY + 1).IsSolid()) {
        if (!circuitSystem || !circuitSystem->IsSolid(gridX, gridY + 1)) {
            gridY++;
        }
    }
}

void Player::MoveLeft() {
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime).count() < moveDelayMs) return;
    direction = LEFT;
    lastMoveTime = now;
    isMoving = true; // Ativa animação
    if (tileMap && !tileMap->GetTile(gridX - 1, gridY).IsSolid()) {
        if (!circuitSystem || !circuitSystem->IsSolid(gridX - 1, gridY)) {
            gridX--;
        }
    }
}

void Player::MoveRight() {
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime).count() < moveDelayMs) return;
    direction = RIGHT;
    lastMoveTime = now;
    isMoving = true; // Ativa animação
    if (tileMap && !tileMap->GetTile(gridX + 1, gridY).IsSolid()) {
        if (!circuitSystem || !circuitSystem->IsSolid(gridX + 1, gridY)) {
            gridX++;
        }
    }
}

void Player::SetMoving(bool moving) {
    isMoving = moving;
}

bool Player::IsMoving() const {
    return isMoving;
}

void Player::Update() {
    // 1. Suavização do Movimento (Interpolação)
    float targetX = gridX * TILE_SIZE;
    float targetY = gridY * TILE_SIZE;
    float lerpSpeed = 0.15f; // Ajuste para mais ou menos fluidez

    worldX += (targetX - worldX) * lerpSpeed;
    worldY += (targetY - worldY) * lerpSpeed;

    // Detectar se chegou perto o suficiente do alvo para parar a animação visual
    bool effectivelyMoving = (std::abs(targetX - worldX) > 0.1f || std::abs(targetY - worldY) > 0.1f);

    // 2. Lógica de Animação
    if (isMoving || effectivelyMoving) {
        frameCounter++;
        if (frameCounter >= 12) { // Aumentado de 8 para 12 para ser mais devagar
            frame = (frame + 1) % FRAMES_PER_DIRECTION;
            frameCounter = 0;
        }
    } else {
        frame = 0; 
        frameCounter = 0;
    }
}

void Player::Render(SDL_Renderer* renderer, int camX, int camY) {
    if (!texture) return;

    SDL_Rect srcRect = {
        frame * FRAME_WIDTH,
        (int)direction * FRAME_HEIGHT,
        FRAME_WIDTH,
        FRAME_HEIGHT
    };

    SDL_Rect destRect = {
        (int)(worldX * SCALE) - camX,
        (int)(worldY * SCALE) - camY,
        FRAME_WIDTH * SCALE,
        FRAME_HEIGHT * SCALE
    };

    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
}

int Player::GetGridX() const { return gridX; }
int Player::GetGridY() const { return gridY; }
float Player::GetWorldX() const { return worldX; }
float Player::GetWorldY() const { return worldY; }
