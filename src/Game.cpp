#include "Game.h"
#include <iostream>
#include <SDL2/SDL_image.h>

Game::Game() : window(nullptr), renderer(nullptr), running(false) {
}

Game::~Game() {
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
    bool moving = false;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    player.MoveUp();
                    moving = true;
                    break;
                case SDLK_DOWN:
                    player.MoveDown();
                    moving = true;
                    break;
                case SDLK_LEFT:
                    player.MoveLeft();
                    moving = true;
                    break;
                case SDLK_RIGHT:
                    player.MoveRight();
                    moving = true;
                    break;
            }
        }
    }

    player.SetMoving(moving);
}

void Game::Update() {
    player.Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    player.Render(renderer);

    SDL_RenderPresent(renderer);
}

void Game::Shutdown() {
    player.Shutdown();

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
