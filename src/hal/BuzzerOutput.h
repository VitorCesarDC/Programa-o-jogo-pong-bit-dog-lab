#pragma once
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "../../include/PinConfig.h"

// Uma nota musical simples: frequência (Hz) e duração (ms).
// freqHz == 0 representa uma pausa/silêncio.
struct Note {
    uint32_t freqHz;
    uint32_t durationMs;
};

// Buzzer é um ATUADOR acionado por eventos (colisão, ponto), não uma
// entidade que se atualiza sozinha a cada frame (pergunta 6).
class BuzzerOutput {
public:
    void init() {
        gpio_set_function(PIN_BUZZER, GPIO_FUNC_PWM);
        slice_ = pwm_gpio_to_slice_num(PIN_BUZZER);
        pwm_set_enabled(slice_, false);
    }

    // Toca um beep bloqueante e simples. Para não travar o game loop em
    // jogos mais exigentes, isso pode virar uma fila de eventos não
    // bloqueante — aqui mantido simples por ser um projeto didático.
    void playBeep(uint32_t freqHz, uint32_t durationMs) {
        uint32_t clock = 125000000;
        uint32_t divider16 = clock / freqHz / 4096 + 1;
        if (divider16 / 16 == 0) divider16 = 16;
        pwm_set_clkdiv(slice_, static_cast<float>(divider16) / 16.0f);

        uint32_t wrap = clock * 16 / divider16 / freqHz - 1;
        pwm_set_wrap(slice_, wrap);
        pwm_set_gpio_level(PIN_BUZZER, wrap / 2); // 50% duty

        pwm_set_enabled(slice_, true);
        sleep_ms(durationMs);
        pwm_set_enabled(slice_, false);
    }

    // Toca um beep "suave": duty cycle baixo em vez de 50%, e curto.
    // Usado no som de colisão, pra não ser tão marcante quanto o de gol.
    void playSoftBeep(uint32_t freqHz, uint32_t durationMs) {
        playBeepWithDuty(freqHz, durationMs, 0.12f);
    }

    // Toca uma sequência de notas, uma após a outra (bloqueante).
    // Bom o bastante aqui pois todo disparo acontece em um momento em que
    // o jogo já está "pausado" (ponto marcado, fim de partida).
    void playMelody(const Note* notes, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            if (notes[i].freqHz == 0) {
                sleep_ms(notes[i].durationMs); // pausa
            } else {
                playBeep(notes[i].freqHz, notes[i].durationMs);
            }
            sleep_ms(15); // pequeno respiro entre notas
        }
    }

private:
    void playBeepWithDuty(uint32_t freqHz, uint32_t durationMs, float duty) {
        uint32_t clock = 125000000;
        uint32_t divider16 = clock / freqHz / 4096 + 1;
        if (divider16 / 16 == 0) divider16 = 16;
        pwm_set_clkdiv(slice_, static_cast<float>(divider16) / 16.0f);

        uint32_t wrap = clock * 16 / divider16 / freqHz - 1;
        pwm_set_wrap(slice_, wrap);
        pwm_set_gpio_level(PIN_BUZZER, static_cast<uint16_t>(wrap * duty));

        pwm_set_enabled(slice_, true);
        sleep_ms(durationMs);
        pwm_set_enabled(slice_, false);
    }

    uint slice_ = 0;
};
