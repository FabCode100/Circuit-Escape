#include "CircuitPuzzle.h"
#include <iostream>
#include <cmath>

CircuitPuzzle::CircuitPuzzle()
    : cursorX(0), cursorY(0), selectedInventory(0),
      active(false), complete(false), lastResult(PuzzleResult::NONE),
      currentPuzzleId(0), resultTimer(0), cursorBlinkTimer(0),
      cursorVisible(true), burnTimer(0),
      componentTexture(nullptr), font(nullptr) {
}

CircuitPuzzle::~CircuitPuzzle() {
    Shutdown();
}

bool CircuitPuzzle::Initialize(SDL_Renderer* renderer) {
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << "\n";
            return false;
        }
    }

    font = TTF_OpenFont("assets/PressStart2P-Regular.ttf", 8);
    if (!font) {
        std::cerr << "Warning: Could not load font: " << TTF_GetError() << "\n";
        // Continua sem fonte - renderizará sem texto
    }

    return true;
}

void CircuitPuzzle::LoadPuzzle(int puzzleId) {
    currentPuzzleId = puzzleId;
    complete = false;
    lastResult = PuzzleResult::NONE;
    resultTimer = 0;
    burnTimer = 0;
    cursorX = 0;
    cursorY = 0;
    selectedInventory = 0;

    // Limpa grid
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            grid[y][x] = PuzzleCell();
        }
    }
    inventory.clear();

    // Configura puzzles diferentes baseado no ID
    if (puzzleId == 1) {
        // Puzzle 1: LED simples - precisa de bateria + resistor + LED + fios
        // Bateria fixa na posição (0, 2)
        grid[2][0].type = ComponentType::BATTERY;
        grid[2][0].isFixed = true;

        // Peças disponíveis para o jogador
        inventory.push_back({ComponentType::WIRE, 4});
        inventory.push_back({ComponentType::RESISTOR, 1});
        inventory.push_back({ComponentType::LED, 1});
    }
}

void CircuitPuzzle::Shutdown() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

void CircuitPuzzle::HandleInput(SDL_Event& event) {
    if (!active) return;

    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                if (cursorY > 0) cursorY--;
                break;
            case SDLK_DOWN:
                if (cursorY < GRID_HEIGHT - 1) cursorY++;
                break;
            case SDLK_LEFT:
                if (cursorX > 0) cursorX--;
                break;
            case SDLK_RIGHT:
                if (cursorX < GRID_WIDTH - 1) cursorX++;
                break;
            case SDLK_SPACE:
            case SDLK_RETURN:
                PlaceComponent();
                break;
            case SDLK_r:
                RotateComponent();
                break;
            case SDLK_BACKSPACE:
            case SDLK_DELETE:
                RemoveComponent();
                break;
            case SDLK_TAB:
                // Ciclando pelo inventário
                if (!inventory.empty()) {
                    selectedInventory = (selectedInventory + 1) % inventory.size();
                }
                break;
            case SDLK_v:
                // Validar circuito
                lastResult = Validate();
                resultTimer = 3.0f; // Mostra resultado por 3 segundos
                if (lastResult == PuzzleResult::NO_RESISTOR) {
                    burnTimer = 2.0f;
                    // Marca LEDs como queimando
                    for (int y = 0; y < GRID_HEIGHT; ++y) {
                        for (int x = 0; x < GRID_WIDTH; ++x) {
                            if (grid[y][x].type == ComponentType::LED) {
                                grid[y][x].isBurning = true;
                            }
                        }
                    }
                }
                if (lastResult == PuzzleResult::CORRECT) {
                    complete = true;
                }
                break;
            case SDLK_ESCAPE:
                Deactivate();
                break;
        }
    }
}

void CircuitPuzzle::Update(float deltaTime) {
    if (!active) return;

    // Animação do cursor piscando
    cursorBlinkTimer += deltaTime;
    if (cursorBlinkTimer >= 0.4f) {
        cursorBlinkTimer = 0;
        cursorVisible = !cursorVisible;
    }

    // Timer de resultado
    if (resultTimer > 0) {
        resultTimer -= deltaTime;
        if (resultTimer <= 0) {
            resultTimer = 0;
            // Reseta visuais de queima
            if (burnTimer > 0) {
                burnTimer = 0;
                for (int y = 0; y < GRID_HEIGHT; ++y) {
                    for (int x = 0; x < GRID_WIDTH; ++x) {
                        grid[y][x].isBurning = false;
                    }
                }
            }
        }
    }

    // Timer de queima
    if (burnTimer > 0) {
        burnTimer -= deltaTime;
    }
}

void CircuitPuzzle::Render(SDL_Renderer* renderer, int screenW, int screenH) {
    if (!active) return;

    // Fundo semi-transparente escuro
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 220);
    SDL_Rect fullscreen = {0, 0, screenW, screenH};
    SDL_RenderFillRect(renderer, &fullscreen);

    int cellSize = 48;
    int gridPixelW = GRID_WIDTH * cellSize;
    int gridPixelH = GRID_HEIGHT * cellSize;
    int offsetX = (screenW - gridPixelW) / 2;
    int offsetY = (screenH - gridPixelH) / 2 - 40;

    // Título "MONTAGEM DE CIRCUITO"
    if (font) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* surf = TTF_RenderText_Solid(font, "MONTAGEM DE CIRCUITO", white);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            int tw = surf->w * 2;
            int th = surf->h * 2;
            SDL_Rect dst = {(screenW - tw) / 2, offsetY - 40, tw, th};
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }

    // Fundo do grid
    SDL_SetRenderDrawColor(renderer, 20, 25, 50, 255);
    SDL_Rect gridBg = {offsetX - 4, offsetY - 4, gridPixelW + 8, gridPixelH + 8};
    SDL_RenderFillRect(renderer, &gridBg);

    // Borda neon do grid
    SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
    SDL_RenderDrawRect(renderer, &gridBg);
    SDL_Rect gridBg2 = {offsetX - 3, offsetY - 3, gridPixelW + 6, gridPixelH + 6};
    SDL_RenderDrawRect(renderer, &gridBg2);

    DrawGrid(renderer, offsetX, offsetY, cellSize);
    DrawCursor(renderer, offsetX, offsetY, cellSize);
    DrawInventory(renderer, offsetX, offsetY + gridPixelH + 20);
    DrawResultFeedback(renderer, screenW, screenH);

    // Instruções na parte inferior - Duas linhas
    if (font) {
        SDL_Color gray = {150, 150, 150, 255};
        const char* inst1 = "[SETAS] Mover  [SPACE] Colocar  [R] Rotacionar";
        const char* inst2 = "[TAB] Trocar  [V] Validar  [ESC] Sair";
        
        // Linha 1
        SDL_Surface* surf1 = TTF_RenderText_Solid(font, inst1, gray);
        if (surf1) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf1);
            SDL_Rect dst = {(screenW - surf1->w) / 2, screenH - 45, surf1->w, surf1->h};
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf1);
        }
        
        // Linha 2
        SDL_Surface* surf2 = TTF_RenderText_Solid(font, inst2, gray);
        if (surf2) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf2);
            SDL_Rect dst = {(screenW - surf2->w) / 2, screenH - 25, surf2->w, surf2->h};
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf2);
        }
    }
}

void CircuitPuzzle::DrawGrid(SDL_Renderer* renderer, int offsetX, int offsetY, int cellSize) {
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            int px = offsetX + x * cellSize;
            int py = offsetY + y * cellSize;

            // Fundo da célula
            SDL_SetRenderDrawColor(renderer, 15, 18, 35, 255);
            SDL_Rect cell = {px + 1, py + 1, cellSize - 2, cellSize - 2};
            SDL_RenderFillRect(renderer, &cell);

            // Borda da célula
            SDL_SetRenderDrawColor(renderer, 40, 50, 80, 255);
            SDL_RenderDrawRect(renderer, &cell);

            // Renderiza componente se houver
            const PuzzleCell& pc = grid[y][x];
            if (pc.type != ComponentType::EMPTY) {
                DrawComponent(renderer, pc.type, pc.rotation,
                             px + 4, py + 4, cellSize - 8,
                             pc.isHighlighted, pc.isBurning);
            }
        }
    }
}

void CircuitPuzzle::DrawComponent(SDL_Renderer* renderer, ComponentType type, int rotation,
                                   int x, int y, int size, bool highlighted, bool burning) {
    int cx = x + size / 2;
    int cy = y + size / 2;
    int half = size / 2 - 2;

    if (burning) {
        // Efeito de queima - vermelho pulsante
        float pulse = std::sin(burnTimer * 10.0f) * 0.5f + 0.5f;
        Uint8 r = (Uint8)(200 + pulse * 55);
        SDL_SetRenderDrawColor(renderer, r, 50, 0, 255);
        SDL_Rect burnRect = {x, y, size, size};
        SDL_RenderFillRect(renderer, &burnRect);
    }

    switch (type) {
        case ComponentType::WIRE: {
            SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
            // Fio horizontal ou vertical dependendo da rotação
            if (rotation == 0 || rotation == 180) {
                SDL_RenderDrawLine(renderer, x, cy, x + size, cy);
                SDL_RenderDrawLine(renderer, x, cy - 1, x + size, cy - 1);
            } else {
                SDL_RenderDrawLine(renderer, cx, y, cx, y + size);
                SDL_RenderDrawLine(renderer, cx - 1, y, cx - 1, y + size);
            }
            break;
        }
        case ComponentType::RESISTOR: {
            // Corpo do resistor - retângulo com listras
            SDL_SetRenderDrawColor(renderer, 200, 150, 50, 255);
            SDL_Rect body = {x + 6, y + 10, size - 12, size - 20};
            SDL_RenderFillRect(renderer, &body);
            // Listras coloridas
            SDL_SetRenderDrawColor(renderer, 150, 50, 50, 255);
            for (int i = 0; i < 3; i++) {
                int sx = x + 10 + i * 8;
                SDL_RenderDrawLine(renderer, sx, y + 10, sx, y + size - 10);
                SDL_RenderDrawLine(renderer, sx + 1, y + 10, sx + 1, y + size - 10);
            }
            // Fios do resistor
            SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
            SDL_RenderDrawLine(renderer, x, cy, x + 6, cy);
            SDL_RenderDrawLine(renderer, x + size - 6, cy, x + size, cy);
            break;
        }
        case ComponentType::LED: {
            // Triângulo do LED
            SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
            for (int i = 0; i < half; i++) {
                int top = cy - i;
                int bot = cy + i;
                SDL_RenderDrawLine(renderer, cx - half + i, top, cx - half + i, bot);
            }
            // Linha do cátodo
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderDrawLine(renderer, cx + half - 4, cy - half, cx + half - 4, cy + half);
            // Fios
            SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
            SDL_RenderDrawLine(renderer, x, cy, cx - half, cy);
            SDL_RenderDrawLine(renderer, cx + half - 4, cy, x + size, cy);
            // Indicador de polaridade (+/-)
            if (font) {
                SDL_Color red = {255, 100, 100, 255};
                SDL_Surface* s = TTF_RenderText_Solid(font, "+", red);
                if (s) {
                    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
                    SDL_Rect d = {x + 2, y + 2, 8, 8};
                    SDL_RenderCopy(renderer, t, nullptr, &d);
                    SDL_DestroyTexture(t);
                    SDL_FreeSurface(s);
                }
            }
            break;
        }
        case ComponentType::BATTERY: {
            // Bateria com + e -
            SDL_SetRenderDrawColor(renderer, 50, 200, 255, 255);
            SDL_Rect body = {x + 8, y + 6, size - 16, size - 12};
            SDL_RenderFillRect(renderer, &body);
            // Polo positivo
            SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
            SDL_Rect plus = {x + size - 10, y + 10, 6, size - 20};
            SDL_RenderFillRect(renderer, &plus);
            // Polo negativo
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_Rect minus = {x + 4, y + 14, 6, size - 28};
            SDL_RenderFillRect(renderer, &minus);
            // Texto
            if (font) {
                SDL_Color yellow = {255, 255, 0, 255};
                SDL_Surface* s = TTF_RenderText_Solid(font, "BAT", yellow);
                if (s) {
                    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
                    SDL_Rect d = {cx - 10, cy - 4, 20, 8};
                    SDL_RenderCopy(renderer, t, nullptr, &d);
                    SDL_DestroyTexture(t);
                    SDL_FreeSurface(s);
                }
            }
            break;
        }
        case ComponentType::SWITCH_COMP: {
            SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
            SDL_Rect body = {x + 4, y + 12, size - 8, size - 24};
            SDL_RenderFillRect(renderer, &body);
            break;
        }
        default:
            break;
    }

    // Borda de highlight
    if (highlighted) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 180);
        SDL_Rect hl = {x - 2, y - 2, size + 4, size + 4};
        SDL_RenderDrawRect(renderer, &hl);
    }
}

void CircuitPuzzle::DrawCursor(SDL_Renderer* renderer, int offsetX, int offsetY, int cellSize) {
    if (!cursorVisible) return;

    int px = offsetX + cursorX * cellSize;
    int py = offsetY + cursorY * cellSize;

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect cursor = {px, py, cellSize, cellSize};
    SDL_RenderDrawRect(renderer, &cursor);
    SDL_Rect cursor2 = {px + 1, py + 1, cellSize - 2, cellSize - 2};
    SDL_RenderDrawRect(renderer, &cursor2);
}

void CircuitPuzzle::DrawInventory(SDL_Renderer* renderer, int offsetX, int offsetY) {
    if (!font) return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {0, 255, 255, 255};

    SDL_Surface* label = TTF_RenderText_Solid(font, "PECAS:", white);
    if (label) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, label);
        SDL_Rect dst = {offsetX, offsetY, label->w, label->h};
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(label);
    }

    int ix = offsetX;
    int iy = offsetY + 16;
    for (int i = 0; i < (int)inventory.size(); ++i) {
        const char* name = "";
        switch (inventory[i].type) {
            case ComponentType::WIRE: name = "FIO"; break;
            case ComponentType::RESISTOR: name = "RESISTOR"; break;
            case ComponentType::LED: name = "LED"; break;
            case ComponentType::SWITCH_COMP: name = "CHAVE"; break;
            default: name = "???"; break;
        }

        char buf[64];
        snprintf(buf, sizeof(buf), "%s%s x%d",
                 (i == selectedInventory) ? "> " : "  ",
                 name, inventory[i].count);

        SDL_Color col = (i == selectedInventory) ? cyan : white;
        SDL_Surface* surf = TTF_RenderText_Solid(font, buf, col);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dst = {ix, iy + i * 14, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }
}

void CircuitPuzzle::DrawResultFeedback(SDL_Renderer* renderer, int screenW, int screenH) {
    if (resultTimer <= 0) return;

    const char* msg = "";
    SDL_Color color = {255, 255, 255, 255};

    switch (lastResult) {
        case PuzzleResult::CORRECT:
            msg = "CIRCUITO CORRETO! BEM FEITO!";
            color = {0, 255, 100, 255};
            break;
        case PuzzleResult::NO_RESISTOR:
            msg = "SEM RESISTOR! LED QUEIMOU!";
            color = {255, 50, 0, 255};
            break;
        case PuzzleResult::OPEN_CIRCUIT:
            msg = "CIRCUITO ABERTO!";
            color = {255, 200, 0, 255};
            break;
        case PuzzleResult::INVERTED:
            msg = "POLARIDADE INVERTIDA!";
            color = {255, 100, 255, 255};
            break;
        default:
            return;
    }

    if (!font) return;

    // Fundo da mensagem
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect msgBg = {screenW / 2 - 200, screenH / 2 - 30, 400, 60};
    SDL_RenderFillRect(renderer, &msgBg);

    // Borda colorida
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawRect(renderer, &msgBg);

    SDL_Surface* surf = TTF_RenderText_Solid(font, msg, color);
    if (surf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        int tw = surf->w * 2;
        int th = surf->h * 2;
        SDL_Rect dst = {screenW / 2 - tw / 2, screenH / 2 - th / 2, tw, th};
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);
    }
}

void CircuitPuzzle::PlaceComponent() {
    PuzzleCell& cell = grid[cursorY][cursorX];
    if (cell.isFixed || cell.type != ComponentType::EMPTY) return;
    if (inventory.empty() || selectedInventory >= (int)inventory.size()) return;

    InventoryItem& item = inventory[selectedInventory];
    if (item.count <= 0) return;

    cell.type = item.type;
    cell.rotation = 0;
    item.count--;
}

void CircuitPuzzle::RotateComponent() {
    PuzzleCell& cell = grid[cursorY][cursorX];
    if (cell.isFixed || cell.type == ComponentType::EMPTY) return;

    cell.rotation = (cell.rotation + 90) % 360;
}

void CircuitPuzzle::RemoveComponent() {
    PuzzleCell& cell = grid[cursorY][cursorX];
    if (cell.isFixed || cell.type == ComponentType::EMPTY) return;

    // Devolve ao inventário
    for (auto& item : inventory) {
        if (item.type == cell.type) {
            item.count++;
            break;
        }
    }
    cell.type = ComponentType::EMPTY;
    cell.rotation = 0;
}

PuzzleResult CircuitPuzzle::Validate() {
    // Tenta rastrear o circuito da bateria até completar o loop
    bool hasResistor = false;
    bool ledCorrectPolarity = true;

    bool foundBattery = false;
    bool foundLED = false;
    bool circuitComplete = false;

    // Encontra a bateria
    int batX = -1, batY = -1;
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (grid[y][x].type == ComponentType::BATTERY) {
                batX = x;
                batY = y;
                foundBattery = true;
            }
            if (grid[y][x].type == ComponentType::RESISTOR) hasResistor = true;
            if (grid[y][x].type == ComponentType::LED) foundLED = true;
        }
    }

    if (!foundBattery) return PuzzleResult::OPEN_CIRCUIT;

    // Tenta seguir o caminho da bateria
    // Simplificação: verifica se existe uma cadeia conectada de componentes
    // que inclui bateria + LED + fios, formando um loop
    bool visited[5][7] = {};
    int pathLength = 0;

    // BFS simples para rastrear conectividade
    struct Pos { int x, y; };
    std::vector<Pos> queue;
    queue.push_back({batX, batY});
    visited[batY][batX] = true;

    while (!queue.empty()) {
        Pos cur = queue.front();
        queue.erase(queue.begin());
        pathLength++;

        // Verifica vizinhos
        int dx[] = {0, 0, -1, 1};
        int dy[] = {-1, 1, 0, 0};
        for (int d = 0; d < 4; ++d) {
            int nx = cur.x + dx[d];
            int ny = cur.y + dy[d];
            if (nx < 0 || nx >= GRID_WIDTH || ny < 0 || ny >= GRID_HEIGHT) continue;
            if (visited[ny][nx]) continue;
            if (grid[ny][nx].type == ComponentType::EMPTY) continue;

            visited[ny][nx] = true;
            queue.push_back({nx, ny});
        }
    }

    // Verifica se todos os componentes estão conectados
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (grid[y][x].type != ComponentType::EMPTY && !visited[y][x]) {
                return PuzzleResult::OPEN_CIRCUIT;
            }
        }
    }

    if (!foundLED) return PuzzleResult::OPEN_CIRCUIT;

    // Verifica se tem resistor
    if (!hasResistor) return PuzzleResult::NO_RESISTOR;

    // Verifica polaridade do LED (simplificado: LED deve ter polo + virado para bateria)
    // Verificação simples baseada na rotação
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (grid[y][x].type == ComponentType::LED) {
                // Se LED está invertido (rotação 180)
                if (grid[y][x].rotation == 180) {
                    return PuzzleResult::INVERTED;
                }
            }
        }
    }

    // Se tem bateria, LED, resistor e está tudo conectado => correto!
    if (pathLength >= 3) {
        circuitComplete = true;
    }

    return circuitComplete ? PuzzleResult::CORRECT : PuzzleResult::OPEN_CIRCUIT;
}

bool CircuitPuzzle::TraceCircuit(bool& hasResistor, bool& ledCorrectPolarity) {
    hasResistor = false;
    ledCorrectPolarity = true;
    return false;
}

bool CircuitPuzzle::IsComplete() const { return complete; }
bool CircuitPuzzle::IsActive() const { return active; }
void CircuitPuzzle::Activate() { active = true; }
void CircuitPuzzle::Deactivate() { active = false; }
PuzzleResult CircuitPuzzle::GetLastResult() const { return lastResult; }
