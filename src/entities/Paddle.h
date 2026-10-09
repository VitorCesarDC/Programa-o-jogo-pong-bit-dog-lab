#pragma once
#include "Entity.h"
#include "IPaddleController.h"

class Paddle : public Entity {
public:
    Paddle(float x, float y, float width, float height, float speed,
           IPaddleController* controller, float screenHeight);

    // ballY: posição atual da bola, repassada pelo GameManager (necessária
    // só para controladores de IA; o controlador humano simplesmente ignora).
    void update(float dt) override { update(dt, y_); } // fallback sem bola
    void update(float dt, float ballY);

    void draw(IRenderer& renderer) const override;

    float getX() const { return x_; }
    float getY() const { return y_; }
    float getHeight() const { return height_; }
    float getWidth() const { return width_; }

private:
    float x_, y_;
    float width_, height_;
    float speed_;
    float screenHeight_;
    IPaddleController* controller_; // não é dono (ver GameManager)
};
