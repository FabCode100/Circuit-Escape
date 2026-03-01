#ifndef VFX_H
#define VFX_H

#include <SDL2/SDL.h>
#include <vector>
#include <cstdlib>

// Partícula de fumaça
struct SmokeParticle {
    float x, y;
    float vx, vy;
    float life;
    float maxLife;
    int size;
    Uint8 alpha;
};

class VFX {
public:
    VFX();
    ~VFX();

    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer, int camX, int camY, int scale);

    // Efeitos disponíveis
    void TriggerScreenShake(float duration, float intensity);
    void TriggerSmoke(int worldX, int worldY, int count = 15);
    void TriggerFlash(SDL_Color color, float duration);

    // Offsets de câmera para screen shake
    int GetShakeOffsetX() const;
    int GetShakeOffsetY() const;

    bool IsFlashing() const;

private:
    // Screen Shake
    bool shaking;
    float shakeDuration;
    float shakeTimer;
    float shakeIntensity;
    int shakeOffsetX;
    int shakeOffsetY;

    // Smoke
    std::vector<SmokeParticle> particles;

    // Flash
    bool flashing;
    float flashDuration;
    float flashTimer;
    SDL_Color flashColor;
};

#endif // VFX_H
