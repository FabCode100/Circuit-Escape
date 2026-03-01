#include "Game.h"
#include <iostream>
#include <cmath>
#include <SDL2/SDL_image.h>
#include "TileMap.h"
#include "Camera.h"

Game::Game()
    : window(nullptr), renderer(nullptr), running(false),
      currentState(GameState::EXPLORATION),
      tilesetTexture(nullptr), objectsTexture(nullptr),
      tileMap(nullptr), camera(nullptr),
      lastFrameTime(0),
      puzzleTriggerX(6), puzzleTriggerY(4),
      puzzleSolved(false), lastPuzzleResult(PuzzleResult::NONE),
      pendingPuzzleId(0), firstTimePuzzle(true) {
}

Game::~Game() {
    if (tileMap) delete tileMap;
    if (camera) delete camera;
    Shutdown();
}

bool Game::Initialize() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << "\n";
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << "\n";
        SDL_Quit();
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "Warning: Failed to initialize SDL_ttf: " << TTF_GetError() << "\n";
        // Não é fatal
    }

    window = SDL_CreateWindow(
        "Circuit Escape - Fase 1",
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
    // Switch de ID 1 na posição 4,4
    circuitSystem.AddElement(CircuitType::SWITCH, 1, 4, 4);
    // Porta de ID 1 na posição 8,4 ligada a esse switch
    circuitSystem.AddElement(CircuitType::DOOR, 1, 8, 4);

    camera = new Camera(SCREEN_WIDTH, SCREEN_HEIGHT);
    player.SetTileMap(tileMap);
    player.SetCircuitSystem(&circuitSystem);

    // Inicializa novos subsistemas da Fase 1
    if (!circuitPuzzle.Initialize(renderer)) {
        std::cerr << "Warning: CircuitPuzzle initialization had issues\n";
    }

    if (!dialogueSystem.Initialize(renderer)) {
        std::cerr << "Warning: DialogueSystem initialization had issues\n";
    }

    audioSystem.Initialize(); // Não é fatal se falhar
    hardwareInterface.Initialize();

    lastFrameTime = SDL_GetTicks();
    running = true;

    // Diálogo introdutório do Professor Leo
    std::vector<std::string> introMessages = {
        "Ola! Eu sou o Professor Leo.",
        "Bem-vindo ao laboratorio de circuitos! Aqui voce vai aprender a montar circuitos eletronicos.",
        "Explore o mapa e encontre as bancadas de trabalho para resolver os desafios!",
        "Cuidado: um LED sem resistor pode QUEIMAR! Use as pecas certas."
    };
    EnterDialogue("Prof. Leo", introMessages);

    return true;
}

void Game::Run() {
    while (running) {
        Uint32 frameStart = SDL_GetTicks();
        float deltaTime = (frameStart - lastFrameTime) / 1000.0f;
        if (deltaTime > 0.1f) deltaTime = 0.1f; // Cap para evitar saltos
        lastFrameTime = frameStart;

        HandleInput();
        
        // Update com deltaTime
        switch (currentState) {
            case GameState::EXPLORATION:
                Update_Exploration();
                break;
            case GameState::PUZZLE:
                Update_Puzzle(deltaTime);
                break;
            case GameState::DIALOGUE:
                Update_Dialogue(deltaTime);
                break;
            case GameState::VICTORY:
                Update_Victory(deltaTime);
                break;
        }

        // VFX sempre atualiza
        vfx.Update(deltaTime);

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
            return;
        }

        // Dispatch por estado
        switch (currentState) {
            case GameState::EXPLORATION:
                if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                    if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_e) {
                        // Checa se está perto do puzzle trigger
                        int px = player.GetGridX();
                        int py = player.GetGridY();
                        int dx = std::abs(px - puzzleTriggerX);
                        int dy = std::abs(py - puzzleTriggerY);
                        if (dx + dy <= 1 && !puzzleSolved) {
                            if (firstTimePuzzle) {
                                pendingPuzzleId = 1;
                                std::vector<std::string> tutor = {
                                    "Berta, vamos montar seu primeiro circuito!",
                                    "O objetivo e fazer a energia sair da BATERIA e chegar no LED.",
                                    "Mas cuidado! O LED e sensivel e precisa de um RESISTOR no caminho.",
                                    "Se a energia for muito forte, o LED vai queimar!",
                                    "Use as [SETAS] para mover e [SPACE] para colocar as pecas do inventario.",
                                    "Nao esqueca de alinhar os FIOS usando a tecla [R] para rotacionar.",
                                    "Quando achar que terminou, aperte [V] para VALIDAR seu circuito. Boa sorte!"
                                };
                                EnterDialogue("Prof. Leo", tutor);
                                firstTimePuzzle = false;
                            } else {
                                EnterPuzzle(1);
                            }
                            return;
                        }
                        // Interação normal com circuitos do mapa
                        circuitSystem.Interact(px, py);
                    }
                }
                break;

            case GameState::PUZZLE:
                circuitPuzzle.HandleInput(event);
                if (!circuitPuzzle.IsActive()) {
                    // Jogador saiu do puzzle (ESC)
                    ExitPuzzle();
                }
                // Verifica resultado
                if (circuitPuzzle.GetLastResult() != PuzzleResult::NONE &&
                    circuitPuzzle.GetLastResult() != lastPuzzleResult) {
                    lastPuzzleResult = circuitPuzzle.GetLastResult();
                    HandlePuzzleResult(lastPuzzleResult);
                }
                break;

            case GameState::DIALOGUE:
                if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                    if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) {
                        dialogueSystem.Advance();
                        if (!dialogueSystem.IsActive()) {
                            // Se for o diálogo final (após puzzleSolved), entra em Victory
                            if (puzzleSolved) {
                                EnterVictory();
                            } else {
                                ExitDialogue();
                            }
                        }
                    }
                }
                break;

            case GameState::VICTORY:
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) {
                        running = false; // Sai do jogo
                    }
                }
                break;
        }
    }

    // Input contínuo apenas no modo exploração
    if (currentState == GameState::EXPLORATION) {
        HandleInput_Exploration();
    }
}

void Game::HandleInput_Exploration() {
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

void Game::HandleInput_Puzzle() {
    // Handled via event dispatch
}

void Game::HandleInput_Dialogue() {
    // Handled via event dispatch
}

void Game::Update() {
    // Chamado pelo Run() via dispatch
}

void Game::Update_Exploration() {
    player.Update();
    circuitSystem.Update();

    // Centralizar câmera no player
    int camX = player.GetWorldX() * SCALE + (TILE_SIZE * SCALE) / 2 - SCREEN_WIDTH / 2;
    int camY = player.GetWorldY() * SCALE + (TILE_SIZE * SCALE) / 2 - SCREEN_HEIGHT / 2;
    int maxCamX = tileMap->GetWidth() * TILE_SIZE * SCALE - SCREEN_WIDTH;
    int maxCamY = tileMap->GetHeight() * TILE_SIZE * SCALE - SCREEN_HEIGHT;
    if (camX < 0) camX = 0;
    if (camY < 0) camY = 0;
    if (camX > maxCamX) camX = maxCamX;
    if (camY > maxCamY) camY = maxCamY;
    camera->SetPosition(camX, camY);
}

void Game::Update_Puzzle(float deltaTime) {
    circuitPuzzle.Update(deltaTime);
}

void Game::Update_Dialogue(float deltaTime) {
    dialogueSystem.Update(deltaTime);
}

void Game::Update_Victory(float deltaTime) {
}

void Game::Render() {
    // Aplica screen shake offset
    int shakeX = vfx.GetShakeOffsetX();
    int shakeY = vfx.GetShakeOffsetY();

    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    // Sempre renderiza o mapa de fundo
    int camX = camera->GetX() + shakeX;
    int camY = camera->GetY() + shakeY;
    tileMap->Render(renderer, camX, camY, SCALE);
    circuitSystem.Render(renderer, camX, camY, SCALE);

    // Renderiza indicador do puzzle trigger
    if (!puzzleSolved) {
        int ptDrawX = puzzleTriggerX * 32 * SCALE - camX;
        int ptDrawY = puzzleTriggerY * 32 * SCALE - camY;
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // Quadrado pulsante amarelo
        Uint32 ticks = SDL_GetTicks();
        float pulse = (float)(std::sin(ticks / 300.0) * 0.3 + 0.7);
        Uint8 alpha = (Uint8)(150 * pulse);
        SDL_SetRenderDrawColor(renderer, 255, 200, 0, alpha);
        SDL_Rect trigRect = {ptDrawX + 4, ptDrawY + 4, 32 * SCALE - 8, 32 * SCALE - 8};
        SDL_RenderDrawRect(renderer, &trigRect);
        SDL_Rect trigRect2 = {ptDrawX + 6, ptDrawY + 6, 32 * SCALE - 12, 32 * SCALE - 12};
        SDL_RenderDrawRect(renderer, &trigRect2);
    }

    player.Render(renderer, camX, camY);

    // VFX em cima de tudo
    vfx.Render(renderer, camX, camY, SCALE);

    // Overlay de estado
    switch (currentState) {
        case GameState::PUZZLE:
            circuitPuzzle.Render(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
            break;
        case GameState::DIALOGUE:
            dialogueSystem.Render(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
            break;
        case GameState::VICTORY:
            Render_Victory();
            break;
        default:
            break;
    }

    SDL_RenderPresent(renderer);
}

void Game::EnterPuzzle(int puzzleId) {
    currentState = GameState::PUZZLE;
    circuitPuzzle.LoadPuzzle(puzzleId);
    circuitPuzzle.Activate();
    lastPuzzleResult = PuzzleResult::NONE;
    std::cout << "[GAME] Entering puzzle " << puzzleId << "\n";
}

void Game::ExitPuzzle() {
    currentState = GameState::EXPLORATION;
    circuitPuzzle.Deactivate();
    std::cout << "[GAME] Exiting puzzle\n";
}

void Game::EnterDialogue(const std::string& speaker, const std::vector<std::string>& messages) {
    currentState = GameState::DIALOGUE;
    dialogueSystem.ShowMessages(speaker, messages);
}

void Game::ExitDialogue() {
    currentState = GameState::EXPLORATION;
    std::cout << "[GAME] Dialogue finished\n";
    
    // Se havia um puzzle esperando o fim do diálogo (tutorial)
    if (pendingPuzzleId > 0 && !puzzleSolved) {
        int id = pendingPuzzleId;
        pendingPuzzleId = 0;
        EnterPuzzle(id);
    }
}

void Game::EnterVictory() {
    currentState = GameState::VICTORY;
    audioSystem.SetVolume(128);
    audioSystem.PlaySuccess();
    std::cout << "[GAME] Victory State Entered\n";
}

void Game::Render_Victory() {
    // Fundo preto semi-transparente
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 230);
    SDL_Rect screen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &screen);

    // Borda Neon dourada
    SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
    SDL_Rect border = {50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100};
    SDL_RenderDrawRect(renderer, &border);

    // Texto de Vitória
    TTF_Font* font = TTF_OpenFont("assets/PressStart2P-Regular.ttf", 20);
    if (font) {
        SDL_Color gold = {255, 215, 0, 255};
        SDL_Surface* surf = TTF_RenderText_Solid(font, "FASE 1 COMPLETA!", gold);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dst = {SCREEN_WIDTH/2 - surf->w/2, 100, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
        TTF_CloseFont(font);
    }

    TTF_Font* smallFont = TTF_OpenFont("assets/PressStart2P-Regular.ttf", 10);
    if (smallFont) {
        SDL_Color white = {255, 255, 255, 255};
        const char* msgs[] = {
            "Voce provou ser um mestre dos circuitos!",
            "O Professor Leo esta impressionado.",
            "",
            "Pressione ESPACO para Sair"
        };
        for(int i=0; i<4; i++) {
            SDL_Surface* surf = TTF_RenderText_Solid(smallFont, msgs[i], white);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect dst = {SCREEN_WIDTH/2 - surf->w/2, 180 + i*25, surf->w, surf->h};
                SDL_RenderCopy(renderer, tex, nullptr, &dst);
                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surf);
            }
        }
        TTF_CloseFont(smallFont);
    }
}

void Game::HandlePuzzleResult(PuzzleResult result) {
    switch (result) {
        case PuzzleResult::CORRECT: {
            std::cout << "[GAME] Puzzle CORRECT!\n";
            audioSystem.PlaySuccess();
            hardwareInterface.SetLEDColor("green");
            hardwareInterface.PlayBuzzerPattern("success");
            vfx.TriggerFlash({0, 255, 100, 150}, 0.5f);
            puzzleSolved = true;

            // Diálogo de parabéns
            std::vector<std::string> msgs = {
                "Excelente trabalho! O circuito esta funcionando perfeitamente!",
                "O resistor protege o LED de queimar. Lembre-se: sempre calcule a resistencia necessaria!",
                "Voce desbloqueou a passagem. Continue explorando!"
            };
            ExitPuzzle();
            EnterDialogue("Prof. Leo", msgs);
            break;
        }
        case PuzzleResult::NO_RESISTOR: {
            std::cout << "[GAME] Puzzle FAIL: No resistor!\n";
            audioSystem.PlayExplosion();
            hardwareInterface.SetLEDColor("red");
            hardwareInterface.PlayBuzzerPattern("error");
            // VFX: fumaça + screen shake
            vfx.TriggerScreenShake(0.5f, 8.0f);
            vfx.TriggerSmoke(puzzleTriggerX * 32 + 16, puzzleTriggerY * 32 + 16, 20);
            vfx.TriggerFlash({255, 50, 0, 120}, 0.3f);
            break;
        }
        case PuzzleResult::OPEN_CIRCUIT: {
            std::cout << "[GAME] Puzzle FAIL: Open circuit!\n";
            audioSystem.PlayError();
            hardwareInterface.SetLEDColor("yellow");
            hardwareInterface.PlayBuzzerPattern("warning");
            break;
        }
        case PuzzleResult::INVERTED: {
            std::cout << "[GAME] Puzzle FAIL: Inverted polarity!\n";
            audioSystem.PlayError();
            hardwareInterface.SetLEDColor("blue");
            hardwareInterface.PlayBuzzerPattern("error");
            vfx.TriggerScreenShake(0.3f, 4.0f);
            break;
        }
        default:
            break;
    }
}

void Game::Shutdown() {
    player.Shutdown();
    circuitSystem.Shutdown();
    circuitPuzzle.Shutdown();
    dialogueSystem.Shutdown();
    audioSystem.Shutdown();
    hardwareInterface.Shutdown();

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

    TTF_Quit();
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
}
