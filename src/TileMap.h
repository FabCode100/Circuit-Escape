#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <string>
#include "Tile.h"

class TileMap {
public:
    TileMap(); // Para iniciar vazio e redimensionar no LoadFromFile
    TileMap(int width, int height);
    void SetTile(int x, int y, int id, bool solid = false);
    bool LoadFromFile(const std::string& filename);
    const Tile& GetTile(int x, int y) const;
    void SetTilesetTexture(SDL_Texture* tex);
    void Render(SDL_Renderer* renderer, int camX, int camY, int scale = 1) const;
    int GetWidth() const;
    int GetHeight() const;
private:
    int width, height;
    SDL_Texture* tilesetTexture;
    std::vector<Tile> tiles;
};

#endif // TILEMAP_H
