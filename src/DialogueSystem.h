#ifndef DIALOGUE_SYSTEM_H
#define DIALOGUE_SYSTEM_H

#include <SDL2/SDL.h>

class DialogueSystem {
public:
    DialogueSystem();
    ~DialogueSystem();

    void Initialize();
    void Update();
    void Render(SDL_Renderer* renderer);
    void Shutdown();

private:
    // Dialogue system implementation
};

#endif // DIALOGUE_SYSTEM_H
