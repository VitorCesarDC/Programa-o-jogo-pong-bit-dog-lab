#include "Ball.h"
#include <cmath>
#include <algorithm>

Ball::Ball(float startX, float startY, float radius, float speed)
    : x_(startX), y_(startY), vx_(speed), vy_(0.0f),
      radius_(radius), baseSpeed_(speed) {}

void Ball::update(float dt) {
    x_ += vx_ * dt;
    y_ += vy_ * dt;
}

void Ball::draw(IRenderer& renderer) const {
    renderer.drawFilledCircle(static_cast<int>(x_), static_cast<int>(y_),
                               static_cast<int>(radius_));
}

void Ball::reset(float x, float y, int direction) {
    x_ = x;
    y_ = y;
    vx_ = baseSpeed_ * static_cast<float>(direction);
    vy_ = 0.0f;
}

void Ball::bounceOffWallVertical() {
    // Colisão com topo/base: só inverte vy. Reação simples (pergunta 4).
    vy_ = -vy_;
}

void Ball::bounceOffPaddle(float paddleCenterY, float paddleHalfHeight) {
    // Colisão com raquete: inverte vx E ajusta vy conforme onde bateu.
    // Isso é diferente da colisão com parede — reação NÃO é igual para
    // todo par de colisão (pergunta 4).
    vx_ = -vx_;

    float offset = (y_ - paddleCenterY) / paddleHalfHeight; // -1.0 .. 1.0
    offset = std::clamp(offset, -1.0f, 1.0f);

    float speed = std::sqrt(vx_ * vx_ + vy_ * vy_);
    if (speed < baseSpeed_) speed = baseSpeed_;

    vy_ = offset * speed * 0.75f;
    vx_ = (vx_ > 0 ? 1.0f : -1.0f) * std::sqrt(std::max(0.0f, speed * speed - vy_ * vy_));
}
