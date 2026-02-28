#include "TileMap.h"
#include <fstream>
#include <sstream>
#include <iostream>

TileMap::TileMap() : width(0), height(0), tilesetTexture(nullptr) {}

TileMap::TileMap(int width, int height) : width(width), height(height), tilesetTexture(nullptr), tiles(width * height) {}

void TileMap::SetTile(int x, int y, int id, bool solid) {
    if (x >= 0 && x < width && y >= 0 && y < height)
        tiles[y * width + x] = Tile(id, solid);
}

bool TileMap::LoadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Falha ao abrir arquivo de mapa: " << filename << "\n";
        return false;
    }

    std::vector<std::vector<int>> tempGrid;
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string val;
        while (std::getline(ss, val, ',')) {
            try {
                row.push_back(std::stoi(val));
            } catch (...) {
                row.push_back(0); // Em caso de erro, coloca chão vazio
            }
        }
        if (!row.empty()) {
            tempGrid.push_back(row);
        }
    }
    file.close();

    if (tempGrid.empty()) return false;

    height = tempGrid.size();
    width = tempGrid[0].size();
    tiles.resize(width * height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Evita out of bounds se tiver alguma linha menor
            int id = (x < tempGrid[y].size()) ? tempGrid[y][x] : 0;
            
            // Regra básica: Se for ID 16 ou maior na nossa spritesheet, é sólido (ex: paredes)
            bool solid = (id >= 16); 
            SetTile(x, y, id, solid);
        }
    }
    return true;
}

const Tile& TileMap::GetTile(int x, int y) const {
    static Tile empty;
    if (x >= 0 && x < width && y >= 0 && y < height)
        return tiles[y * width + x];
    return empty;
}

void TileMap::SetTilesetTexture(SDL_Texture* tex) {
    tilesetTexture = tex;
}

void TileMap::Render(SDL_Renderer* renderer, int camX, int camY, int scale) const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int drawX = x * 32 * scale - camX;
            int drawY = y * 32 * scale - camY;
            tiles[y * width + x].Render(renderer, tilesetTexture, drawX, drawY, scale);
        }
    }
}

int TileMap::GetWidth() const { return width; }
int TileMap::GetHeight() const { return height; }
