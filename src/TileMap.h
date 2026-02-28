#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include "Tile.h"

class TileMap {
public:
    TileMap(int width, int height);
    void SetTile(int x, int y, int id, bool solid = false);
    const Tile& GetTile(int x, int y) const;
    void Render(SDL_Renderer* renderer, int camX, int camY, int scale = 1) const;
    int GetWidth() const;
    int GetHeight() const;
private:
    int width, height;
    std::vector<Tile> tiles;
};

#endif // TILEMAP_H
