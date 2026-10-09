#pragma once
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include "../../include/PinConfig.h"

// Matriz 5x5 de LEDs WS2812B da BitDogLab. Usada só para as comemorações
// "grandes" (vitória/derrota) — efeitos rápidos (gol) usam o LED RGB único.
class NeoMatrixOutput {
public:
    static constexpr int WIDTH = 5;
    static constexpr int HEIGHT = 5;
    static constexpr int NUM_PIXELS = WIDTH * HEIGHT;

    void init() {
        pio_ = pio0;
        uint offset = pio_add_program(pio_, &ws2812_program);
        sm_ = pio_claim_unused_sm(pio_, true);
        ws2812_program_init(pio_, sm_, offset, PIN_NEOPIXEL, 800000.0f);
        clear();
        show();
    }

    void clear() {
        for (auto& p : pixels_) p = 0;
    }

    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
        // Matriz costuma ser "serpentina" (zig-zag) fisicamente — ajuste se
        // a sua placa não seguir esse padrão de fiação.
        int index = (y % 2 == 0) ? (y * WIDTH + x) : (y * WIDTH + (WIDTH - 1 - x));
        pixels_[index] = packColor(r, g, b);
    }

    void fill(uint8_t r, uint8_t g, uint8_t b) {
        uint32_t c = packColor(r, g, b);
        for (auto& p : pixels_) p = c;
    }

    void show() {
        for (uint32_t p : pixels_) {
            pio_sm_put_blocking(pio_, sm_, p << 8u); // GRB nos 24 bits altos
        }
        sleep_us(60); // latch/reset da WS2812 (>50us)
    }

private:
    static uint32_t packColor(uint8_t r, uint8_t g, uint8_t b) {
        return (static_cast<uint32_t>(g) << 16) |
               (static_cast<uint32_t>(r) << 8) |
               static_cast<uint32_t>(b);
    }

    PIO pio_ = nullptr;
    uint sm_ = 0;
    uint32_t pixels_[NUM_PIXELS] = {0};
};
