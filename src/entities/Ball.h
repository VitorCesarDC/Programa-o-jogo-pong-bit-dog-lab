#pragma once
#include "Entity.h"

class Paddle; // forward declaration — Ball não precisa conhecer Paddle por completo

class Ball : public Entity {
public:
    Ball(float startX, float startY, float radius, float speed);

    void update(float dt) override;
    void draw(IRenderer& renderer) const override;

    void reset(float x, float y, int direction /* -1 esquerda, +1 direita */);

    // Chamado pelo GameManager/CollisionSystem quando detecta colisão —
    // a bola não sabe sozinha que colidiu, alguém de fora avisa (pergunta 5).
    void bounceOffWallVertical();
    void bounceOffPaddle(float paddleCenterY, float paddleHalfHeight);

    float getX() const { return x_; }
    float getY() const { return y_; }
    float getRadius() const { return radius_; }
    float getVx() const { return vx_; }
    float getVy() const { return vy_; }

private:
    float x_, y_;
    float vx_, vy_;
    float radius_;
    float baseSpeed_;
};
