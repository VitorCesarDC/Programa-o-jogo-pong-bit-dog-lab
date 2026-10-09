#pragma once
#include "IPaddleController.h"
#include "../hal/JoystickInput.h"

// Controlador humano: lê o joystick físico. Não sabe nada sobre a bola,
// por isso ignora ballY — mostra bem que a interface é genérica o
// suficiente para os dois casos de uso (pergunta 7).
class HumanController : public IPaddleController {
public:
    explicit HumanController(JoystickInput* joystick) : joystick_(joystick) {}

    float getVerticalInput(float /*ownY*/, float /*ballY*/) override {
        return joystick_->readNormalizedY(); // -1.0 (cima) .. 1.0 (baixo)
    }

private:
    JoystickInput* joystick_;
};
