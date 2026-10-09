#pragma once
#include "IPaddleController.h"
#include <cmath>

// IA simples: persegue a posição Y da bola, com uma "zona morta" para não
// tremer perto do alvo. Recebe ballY do GameManager a cada frame — não
// guarda ponteiro para a Ball (pergunta 5).
class AiController : public IPaddleController {
public:
    explicit AiController(float deadZone = 5.0f, float responsiveness = 0.60f)
        : deadZone_(deadZone), responsiveness_(responsiveness) {}

    float getVerticalInput(float ownY, float ballY) override {
        float diff = ballY - ownY;
        if (std::fabs(diff) < deadZone_) return 0.0f;
        float direction = diff > 0 ? 1.0f : -1.0f;
        return direction * responsiveness_;
    }

private:
    float deadZone_;
    float responsiveness_;
};
