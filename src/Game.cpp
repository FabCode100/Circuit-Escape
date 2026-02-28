#include "Game.h"
#include <iostream>
#include <SDL2/SDL_image.h>
#include "TileMap.h"
#include "Camera.h"

Game::Game() : window(nullptr), renderer(nullptr), running(false), tilesetTexture(nullptr), objectsTexture(nullptr), tileMap(nullptr), camera(nullptr) {
}

Game::~Game() {
    if (tileMap) delete tileMap;
    if (camera) delete camera;
    Shutdown();
}

bool Game::Initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << "\n";
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << "\n";
        SDL_Quit();
        return false;
    }

    window = SDL_CreateWindow(
        "Circuit Escape",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );

    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << "\n";
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    if (!player.Initialize(renderer)) {
        std::cerr << "Failed to initialize player\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    SDL_Surface* tileSurface = IMG_Load("assets/tileset_circuit.png");
    if (!tileSurface) {
        std::cerr << "Failed to load tileset_circuit: " << IMG_GetError() << "\n";
        player.Shutdown();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return false;
    }
    tilesetTexture = SDL_CreateTextureFromSurface(renderer, tileSurface);
    SDL_FreeSurface(tileSurface);
    
    if (!tilesetTexture) {
        std::cerr << "Failed to create tileset texture: " << SDL_GetError() << "\n";
        return false;
    }

    tileMap = new TileMap();
    if (!tileMap->LoadFromFile("assets/map1.csv")) {
        std::cerr << "Warning: Could not load assets/map1.csv. Creating fallback map.\n";
        // Fallback map se arquivo não existir
        delete tileMap;
        tileMap = new TileMap(24, 16);
        for (int y = 0; y < 16; ++y) {
            for (int x = 0; x < 24; ++x) {
                bool solid = (x == 0 || y == 0 || x == 23 || y == 15);
                tileMap->SetTile(x, y, solid ? 16 : 0, solid);
            }
        }
    }
    
    tileMap->SetTilesetTexture(tilesetTexture);
    
    // Configurando Circuit System (Objetos do Labirinto)
    SDL_Surface* objSurface = IMG_Load("assets/tileset_objects.png");
    if (!objSurface) {
        std::cerr << "Warning: Could not load assets/tileset_objects.png: " << IMG_GetError() << "\n";
    } else {
        objectsTexture = SDL_CreateTextureFromSurface(renderer, objSurface);
        SDL_FreeSurface(objSurface);
    }
    
    circuitSystem.Initialize(objectsTexture);
    // Adicionando cenário experimental: 
    // Switch de ID 1 na posição 4,4
    circuitSystem.AddElement(CircuitType::SWITCH, 1, 4, 4);
    // Porta de ID 1 na posição 8,4 ligada a esse switch
    circuitSystem.AddElement(CircuitType::DOOR, 1, 8, 4);

    camera = new Camera(SCREEN_WIDTH, SCREEN_HEIGHT);
    player.SetTileMap(tileMap);
    player.SetCircuitSystem(&circuitSystem);

    running = true;
    return true;
}

void Game::Run() {
    while (running) {
        Uint32 frameStart = SDL_GetTicks();

        HandleInput();
        Update();
        Render();

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
}

void Game::HandleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        
        // Pressionamento uníco para evitar segurar a tecla e o botão "piscar" loucamente
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_e) {
                // Tenta interagir
                circuitSystem.Interact(player.GetGridX(), player.GetGridY());
            }
        }
    }

    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    bool moving = false;

    if (keystate[SDL_SCANCODE_UP]) {
        player.MoveUp();
        moving = true;
    }
    else if (keystate[SDL_SCANCODE_DOWN]) {
        player.MoveDown();
        moving = true;
    }
    else if (keystate[SDL_SCANCODE_LEFT]) {
        player.MoveLeft();
        moving = true;
    }
    else if (keystate[SDL_SCANCODE_RIGHT]) {
        player.MoveRight();
        moving = true;
    }

    player.SetMoving(moving);
}

void Game::Update() {
    player.Update();
    circuitSystem.Update(); // Sincroniza estado das portas
    
    // Centralizar câmera no player (usando worldX/Y para suavidade)
    int camX = player.GetWorldX() * SCALE + (TILE_SIZE * SCALE) / 2 - SCREEN_WIDTH / 2;
    int camY = player.GetWorldY() * SCALE + (TILE_SIZE * SCALE) / 2 - SCREEN_HEIGHT / 2;
    // Limites máximos
    int maxCamX = tileMap->GetWidth() * TILE_SIZE * SCALE - SCREEN_WIDTH;
    int maxCamY = tileMap->GetHeight() * TILE_SIZE * SCALE - SCREEN_HEIGHT;
    if (camX < 0) camX = 0;
    if (camY < 0) camY = 0;
    if (camX > maxCamX) camX = maxCamX;
    if (camY > maxCamY) camY = maxCamY;
    camera->SetPosition(camX, camY);
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);
    // Renderizar o mapa fixo
    tileMap->Render(renderer, camera->GetX(), camera->GetY(), SCALE);
    // Renderizar itens e circuitos em cima do mapa
    circuitSystem.Render(renderer, camera->GetX(), camera->GetY(), SCALE);
    // Renderizar o player em cima de tudo
    player.Render(renderer, camera->GetX(), camera->GetY());
    SDL_RenderPresent(renderer);
}

void Game::Shutdown() {
    player.Shutdown();
    circuitSystem.Shutdown();

    if (objectsTexture) {
        SDL_DestroyTexture(objectsTexture);
        objectsTexture = nullptr;
    }

    if (tilesetTexture) {
        SDL_DestroyTexture(tilesetTexture);
        tilesetTexture = nullptr;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
}
