#include "Paddle.h"
#include <algorithm>

Paddle::Paddle(float x, float y, float width, float height, float speed,
               IPaddleController* controller, float screenHeight)
    : x_(x), y_(y), width_(width), height_(height),
      speed_(speed), screenHeight_(screenHeight), controller_(controller) {}

void Paddle::update(float dt, float ballY) {
    float input = controller_->getVerticalInput(y_, ballY); // -1..1
    y_ += input * speed_ * dt;

    float halfH = height_ / 2.0f;
    y_ = std::clamp(y_, halfH, screenHeight_ - halfH);
}

void Paddle::draw(IRenderer& renderer) const {
    renderer.drawRect(static_cast<int>(x_ - width_ / 2.0f),
                       static_cast<int>(y_ - height_ / 2.0f),
                       static_cast<int>(width_), static_cast<int>(height_));
}
