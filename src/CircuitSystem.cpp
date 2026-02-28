#include "CircuitSystem.h"
#include <iostream>
#include <cmath>

CircuitSystem::CircuitSystem() : objectTexture(nullptr) {}

CircuitSystem::~CircuitSystem() {
    Shutdown();
}

void CircuitSystem::Initialize(SDL_Texture* objTexture) {
    objectTexture = objTexture;
}

void CircuitSystem::AddElement(CircuitType type, int id, int x, int y) {
    CircuitElement el;
    el.type = type;
    el.id = id;
    el.gridX = x;
    el.gridY = y;
    el.isActive = false; // Começa desligado / fechado
    elements.push_back(el);
}

void CircuitSystem::Update() {
    // Sincroniza portas e energia baseando nos switches
    for (auto& door : elements) {
        if (door.type == CircuitType::DOOR) {
            bool shouldBeOpen = false;
            // Varre procurando se algum switch com a mesma ID está ON
            for (const auto& sw : elements) {
                if (sw.type == CircuitType::SWITCH && sw.id == door.id && sw.isActive) {
                    shouldBeOpen = true;
                    break;
                }
            }
            door.isActive = shouldBeOpen; // Abre porta se o switch ligado achado bate a id
        }
    }
}

void CircuitSystem::Render(SDL_Renderer* renderer, int camX, int camY, int scale) {
    if (!objectTexture) return;

    for (const auto& el : elements) {
        int drawX = el.gridX * 32 * scale - camX;
        int drawY = el.gridY * 32 * scale - camY;

        int spriteId = 0;
        if (el.type == CircuitType::SWITCH) {
            spriteId = el.isActive ? 11 : 10; // ON vs OFF do switch na nossa spritesheet de itens
        } else if (el.type == CircuitType::DOOR) {
            spriteId = el.isActive ? 25 : 24; // ABERTA vs FECHADA 
            if (el.isActive) continue; // Decidindo não renderizar parede sólida ou renderizar chão quebrado quando a porta abre
        }

        int srcX = (spriteId % 8) * 32;
        int srcY = (spriteId / 8) * 32;

        SDL_Rect srcRect = { srcX, srcY, 32, 32 };
        SDL_Rect destRect = { drawX, drawY, 32 * scale, 32 * scale };

        SDL_RenderCopy(renderer, objectTexture, &srcRect, &destRect);
    }
}

void CircuitSystem::Interact(int playerX, int playerY) {
    // Ao apertar Espaço, procura switches num raio de 1 bloco
    for (auto& el : elements) {
        if (el.type == CircuitType::SWITCH) {
            int dx = std::abs(el.gridX - playerX);
            int dy = std::abs(el.gridY - playerY);
            if (dx + dy <= 1) { // 1 tile de distância exata vertical ou horizontal
                el.isActive = !el.isActive;
            }
        }
    }
}

bool CircuitSystem::IsSolid(int x, int y) const {
    for (const auto& el : elements) {
        if (el.gridX == x && el.gridY == y) {
            if (el.type == CircuitType::DOOR && !el.isActive) {
                return true; // Porta fechada bloqueia
            }
            if (el.type == CircuitType::SWITCH) {
                return true; // Máquinas com switch bloqueiam (não pisamos nelas)
            }
        }
    }
    return false;
}

void CircuitSystem::Shutdown() {
    elements.clear();
}
