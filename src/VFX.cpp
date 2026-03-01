#include "VFX.h"
#include <cmath>

VFX::VFX()
    : shaking(false), shakeDuration(0), shakeTimer(0), shakeIntensity(0),
      shakeOffsetX(0), shakeOffsetY(0),
      flashing(false), flashDuration(0), flashTimer(0) {
    flashColor = {255, 255, 255, 255};
}

VFX::~VFX() {}

void VFX::Update(float deltaTime) {
    // Screen Shake
    if (shaking) {
        shakeTimer -= deltaTime;
        if (shakeTimer <= 0) {
            shaking = false;
            shakeOffsetX = 0;
            shakeOffsetY = 0;
        } else {
            float progress = shakeTimer / shakeDuration;
            float currentIntensity = shakeIntensity * progress;
            shakeOffsetX = (int)((rand() % 100 - 50) / 50.0f * currentIntensity);
            shakeOffsetY = (int)((rand() % 100 - 50) / 50.0f * currentIntensity);
        }
    }

    // Smoke particles
    for (int i = (int)particles.size() - 1; i >= 0; --i) {
        SmokeParticle& p = particles[i];
        p.life -= deltaTime;
        if (p.life <= 0) {
            particles.erase(particles.begin() + i);
            continue;
        }
        p.x += p.vx * deltaTime;
        p.y += p.vy * deltaTime;
        p.vy -= 20.0f * deltaTime; // Sobe
        float ratio = p.life / p.maxLife;
        p.alpha = (Uint8)(200 * ratio);
        p.size = (int)(6 + (1.0f - ratio) * 10);
    }

    // Flash
    if (flashing) {
        flashTimer -= deltaTime;
        if (flashTimer <= 0) {
            flashing = false;
        }
    }
}

void VFX::Render(SDL_Renderer* renderer, int camX, int camY, int scale) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Smoke particles
    for (const auto& p : particles) {
        int drawX = (int)(p.x * scale) - camX;
        int drawY = (int)(p.y * scale) - camY;
        int drawSize = p.size * scale / 2;

        SDL_SetRenderDrawColor(renderer, 150, 150, 150, p.alpha);
        SDL_Rect rect = {drawX - drawSize / 2, drawY - drawSize / 2, drawSize, drawSize};
        SDL_RenderFillRect(renderer, &rect);

        // Borda mais clara
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, (Uint8)(p.alpha / 2));
        SDL_Rect rect2 = {drawX - drawSize / 2 - 1, drawY - drawSize / 2 - 1, drawSize + 2, drawSize + 2};
        SDL_RenderDrawRect(renderer, &rect2);
    }

    // Full-screen flash
    if (flashing) {
        float ratio = flashTimer / flashDuration;
        Uint8 a = (Uint8)(flashColor.a * ratio);
        SDL_SetRenderDrawColor(renderer, flashColor.r, flashColor.g, flashColor.b, a);
        SDL_Rect full = {0, 0, 2000, 2000}; // Grande o suficiente
        SDL_RenderFillRect(renderer, &full);
    }
}

void VFX::TriggerScreenShake(float duration, float intensity) {
    shaking = true;
    shakeDuration = duration;
    shakeTimer = duration;
    shakeIntensity = intensity;
}

void VFX::TriggerSmoke(int worldX, int worldY, int count) {
    for (int i = 0; i < count; ++i) {
        SmokeParticle p;
        p.x = (float)worldX + (rand() % 20 - 10);
        p.y = (float)worldY + (rand() % 20 - 10);
        p.vx = (rand() % 60 - 30) * 1.0f;
        p.vy = -(rand() % 40 + 20) * 1.0f;
        p.maxLife = 0.8f + (rand() % 100) / 100.0f * 0.6f;
        p.life = p.maxLife;
        p.size = 4 + rand() % 6;
        p.alpha = 200;
        particles.push_back(p);
    }
}

void VFX::TriggerFlash(SDL_Color color, float duration) {
    flashing = true;
    flashColor = color;
    flashDuration = duration;
    flashTimer = duration;
}

int VFX::GetShakeOffsetX() const { return shakeOffsetX; }
int VFX::GetShakeOffsetY() const { return shakeOffsetY; }
bool VFX::IsFlashing() const { return flashing; }
