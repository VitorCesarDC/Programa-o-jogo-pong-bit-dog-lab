#pragma once
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"

// Botão com pull-up interno e debounce simples por tempo.
// Pressionado = nível baixo (ver pinagem da BitDogLab).
class ButtonInput {
public:
    explicit ButtonInput(uint pin) : pin_(pin) {}

    void init() {
        gpio_init(pin_);
        gpio_set_dir(pin_, GPIO_IN);
        gpio_pull_up(pin_);
    }

    // true só uma vez por "clique" (borda de descida com debounce),
    // não fica true o tempo todo que o botão está pressionado.
    bool wasPressed() {
        bool level = !gpio_get(pin_); // pressionado = LOW
        absolute_time_t now = get_absolute_time();

        if (level && !lastLevel_ &&
            absolute_time_diff_us(lastChangeTime_, now) > DEBOUNCE_US) {
            lastLevel_ = level;
            lastChangeTime_ = now;
            return true;
        }

        if (level != lastLevel_) {
            lastLevel_ = level;
            lastChangeTime_ = now;
        }
        return false;
    }

private:
    static constexpr int64_t DEBOUNCE_US = 30000; // 30ms
    uint pin_;
    bool lastLevel_ = false;
    absolute_time_t lastChangeTime_ = {0};
};
