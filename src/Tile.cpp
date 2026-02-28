#include "Tile.h"

Tile::Tile(int id, bool solid) : id(id), solid(solid) {}

int Tile::GetId() const { return id; }
bool Tile::IsSolid() const { return solid; }
void Tile::Render(SDL_Renderer* renderer, SDL_Texture* tileset, int x, int y, int scale) const {
    SDL_Rect destRect = { x, y, 32 * scale, 32 * scale };

    if (!tileset) {
        // Placeholder render se a textura não estiver carregada
        if (solid)
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        else
            SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
        SDL_RenderFillRect(renderer, &destRect);
        return;
    }

    // Seleciona as coordenadas do tileset (assumindo imagem 8x8 de tiles 32x32)
    int srcX = (id % 8) * 32;
    int srcY = (id / 8) * 32;
    
    SDL_Rect srcRect = { srcX, srcY, 32, 32 };
    SDL_RenderCopy(renderer, tileset, &srcRect, &destRect);
}
