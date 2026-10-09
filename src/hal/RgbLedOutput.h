#pragma once
#include <initializer_list>
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "../../include/PinConfig.h"

// Também um atuador reativo, não uma entidade (pergunta 6): só muda de
// cor quando o GameManager manda, não tem lógica própria por frame.
class RgbLedOutput {
public:
    void init() {
        for (uint pin : {PIN_LED_R, PIN_LED_G, PIN_LED_B}) {
            gpio_set_function(pin, GPIO_FUNC_PWM);
            uint slice = pwm_gpio_to_slice_num(pin);
            pwm_set_wrap(slice, 255);
            pwm_set_enabled(slice, true);
        }
    }

    void setColor(uint8_t r, uint8_t g, uint8_t b) {
        pwm_set_gpio_level(PIN_LED_R, r);
        pwm_set_gpio_level(PIN_LED_G, g);
        pwm_set_gpio_level(PIN_LED_B, b);
    }
};
