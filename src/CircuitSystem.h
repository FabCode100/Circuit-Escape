#ifndef CIRCUIT_SYSTEM_H
#define CIRCUIT_SYSTEM_H

#include <SDL2/SDL.h>

class CircuitSystem {
public:
    CircuitSystem();
    ~CircuitSystem();

    void Initialize();
    void Update();
    void Render(SDL_Renderer* renderer);
    void Shutdown();

private:
    // Circuit system implementation
};

#endif // CIRCUIT_SYSTEM_H
