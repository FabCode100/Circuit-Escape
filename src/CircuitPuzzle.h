#ifndef CIRCUIT_PUZZLE_H
#define CIRCUIT_PUZZLE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

// Tipos de componentes que o jogador pode colocar no grid
enum class ComponentType {
    EMPTY,
    WIRE,         // Fio condutor
    RESISTOR,     // Resistor
    LED,          // LED (tem polaridade)
    BATTERY,      // Fonte de energia (já posicionada)
    SWITCH_COMP   // Chave liga/desliga
};

// Resultado da validação do circuito
enum class PuzzleResult {
    NONE,
    CORRECT,         // Circuito montado corretamente
    NO_RESISTOR,     // LED sem resistor -> queima (fumaça!)
    OPEN_CIRCUIT,    // Circuito aberto -> nada acontece
    INVERTED         // Polaridade invertida do LED
};

// Cada célula do grid de puzzle
struct PuzzleCell {
    ComponentType type;
    int rotation;           // 0, 90, 180, 270 graus
    bool isFixed;           // true = peça já posicionada (não pode mover)
    bool isHighlighted;     // feedback visual
    bool isBurning;         // animação de queima

    PuzzleCell() : type(ComponentType::EMPTY), rotation(0), 
                   isFixed(false), isHighlighted(false), isBurning(false) {}
};

class CircuitPuzzle {
public:
    CircuitPuzzle();
    ~CircuitPuzzle();

    bool Initialize(SDL_Renderer* renderer);
    void LoadPuzzle(int puzzleId);
    void Shutdown();

    void HandleInput(SDL_Event& event);
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer, int screenW, int screenH);

    PuzzleResult Validate();
    bool IsComplete() const;
    bool IsActive() const;

    void Activate();
    void Deactivate();

    PuzzleResult GetLastResult() const;

private:
    // Grid do puzzle
    static const int GRID_WIDTH = 7;
    static const int GRID_HEIGHT = 5;
    PuzzleCell grid[5][7];

    // Cursor do jogador
    int cursorX;
    int cursorY;

    // Inventário de peças disponíveis
    struct InventoryItem {
        ComponentType type;
        int count;
    };
    std::vector<InventoryItem> inventory;
    int selectedInventory;

    // Estado do puzzle
    bool active;
    bool complete;
    PuzzleResult lastResult;
    int currentPuzzleId;

    // Animações
    float resultTimer;
    float cursorBlinkTimer;
    bool cursorVisible;
    float burnTimer;

    // Texturas/Renderização
    SDL_Texture* componentTexture;
    TTF_Font* font;

    // Métodos internos
    void DrawGrid(SDL_Renderer* renderer, int offsetX, int offsetY, int cellSize);
    void DrawComponent(SDL_Renderer* renderer, ComponentType type, int rotation,
                       int x, int y, int size, bool highlighted, bool burning);
    void DrawCursor(SDL_Renderer* renderer, int offsetX, int offsetY, int cellSize);
    void DrawInventory(SDL_Renderer* renderer, int offsetX, int offsetY);
    void DrawResultFeedback(SDL_Renderer* renderer, int screenW, int screenH);
    void PlaceComponent();
    void RotateComponent();
    void RemoveComponent();
    bool TraceCircuit(bool& hasResistor, bool& ledCorrectPolarity);
};

#endif // CIRCUIT_PUZZLE_H
