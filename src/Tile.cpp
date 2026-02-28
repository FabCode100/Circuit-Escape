#include "Tile.h"

Tile::Tile(int id, bool solid) : id(id), solid(solid) {}

int Tile::GetId() const { return id; }
bool Tile::IsSolid() const { return solid; }
void Tile::Render(SDL_Renderer* renderer, int x, int y, int scale) const {
    // Placeholder: render a colored rect for now
    SDL_Rect rect = { x, y, 32 * scale, 32 * scale };
    if (solid)
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    else
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderFillRect(renderer, &rect);
}
