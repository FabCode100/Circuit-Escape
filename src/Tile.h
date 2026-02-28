#ifndef TILE_H
#define TILE_H

#include <SDL2/SDL.h>

class Tile {
public:
    Tile(int id = 0, bool solid = false);
    int GetId() const;
    bool IsSolid() const;
    void Render(SDL_Renderer* renderer, int x, int y, int scale = 1) const;
private:
    int id;
    bool solid;
};

#endif // TILE_H
