#include <SDL2/SDL.h>
#include <iostream>
#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;
    
    if (!game.Initialize()) {
        std::cerr << "Failed to initialize game\n";
        return 1;
    }

    game.Run();
    game.Shutdown();

    return 0;
}
