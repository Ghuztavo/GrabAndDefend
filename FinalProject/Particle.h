#pragma once
#include <raylib.h>

class Particle {
public:
    Vector2 position;
    Vector2 velocity;
    float lifeTime;
    float maxLifeTime;
    Color color;
    float size;

    Particle(Vector2 pos, Vector2 vel, float life, Color col, float sz) {
        position = pos;
        velocity = vel;
        lifeTime = life;
        maxLifeTime = life;
        color = col;
        size = sz;
    }

    void Update(float deltaTime) {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        lifeTime -= deltaTime;

        // Fade out as lifetime decreases
        color.a = (unsigned char)((lifeTime / maxLifeTime) * 255);
    }

    void Draw() const {
        DrawCircleV(position, size, color);
    }

    bool IsAlive() const {
        return lifeTime > 0;
    }
};
