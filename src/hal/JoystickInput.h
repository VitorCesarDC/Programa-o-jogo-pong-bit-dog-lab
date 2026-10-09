#pragma once
#include <cmath>
#include "hardware/adc.h"
#include "../../include/PinConfig.h"

// Encapsula a leitura do ADC do joystick, já normalizada e com zona morta.
// Isso isola o resto do jogo do detalhe de "ADC vai de 0 a 4095".
class JoystickInput {
public:
    void init() {
        adc_init();
        adc_gpio_init(PIN_JOY_Y);
        adc_gpio_init(PIN_JOY_X);
        // Calibração simples: assume centro em ~2048. Para maior precisão,
        // leia o valor de repouso no init() e use como offset.
    }

    // Retorna -1.0 (para cima) a 1.0 (para baixo), com zona morta central.
    float readNormalizedY() {
        adc_select_input(0); // ADC0 = GPIO26 = PIN_JOY_Y
        uint16_t raw = adc_read(); // 0..4095
        float centered = (static_cast<float>(raw) - 2048.0f) / 2048.0f; // -1..1

        constexpr float DEAD_ZONE = 0.12f;
        if (std::fabs(centered) < DEAD_ZONE) return 0.0f;
        return -centered; // Invertido para que mover para cima suba a raquete (-Y)
    }

    // Para navegação em MENU (discreta, não contínua): retorna -1 (cima),
    // +1 (baixo) ou 0, só UMA VEZ por movimento — precisa voltar ao centro
    // antes de contar um novo passo, senão o menu "correria" sozinho.
    int readMenuStep() {
        float y = readNormalizedY();
        constexpr float STEP_THRESHOLD = 0.6f;

        int direction = 0;
        if (y < -STEP_THRESHOLD) direction = -1;
        else if (y > STEP_THRESHOLD) direction = 1;

        int result = 0;
        if (direction != 0 && !stepArmed_) {
            result = direction;
            stepArmed_ = true;
        } else if (direction == 0) {
            stepArmed_ = false; // voltou ao centro, libera o próximo passo
        }
        return result;
    }

private:
    bool stepArmed_ = false;
};
