#include "TileMap.h"

TileMap::TileMap(int width, int height) : width(width), height(height), tiles(width * height) {}

void TileMap::SetTile(int x, int y, int id, bool solid) {
    if (x >= 0 && x < width && y >= 0 && y < height)
        tiles[y * width + x] = Tile(id, solid);
}

const Tile& TileMap::GetTile(int x, int y) const {
    static Tile empty;
    if (x >= 0 && x < width && y >= 0 && y < height)
        return tiles[y * width + x];
    return empty;
}

void TileMap::Render(SDL_Renderer* renderer, int camX, int camY, int scale) const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int drawX = x * 32 * scale - camX;
            int drawY = y * 32 * scale - camY;
            tiles[y * width + x].Render(renderer, drawX, drawY, scale);
        }
    }
}

int TileMap::GetWidth() const { return width; }
int TileMap::GetHeight() const { return height; }
