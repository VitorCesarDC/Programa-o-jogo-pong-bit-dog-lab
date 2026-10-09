#pragma once
#include "../entities/Entity.h"
#include "../../include/PinConfig.h"

// IMPORTANTE: este arquivo assume uma lib ssd1306 estilo curso (com funções
// como ssd1306_init, ssd1306_clear, ssd1306_draw_pixel, ssd1306_show).
// Troque as chamadas abaixo pelas funções exatas da biblioteca fornecida
// na disciplina, se o nome for diferente. O ponto importante da
// arquitetura é: o resto do jogo (Ball, Paddle, GameManager) NUNCA chama
// a lib do display diretamente — só conhece a interface IRenderer.
extern "C" {
    #include "ssd1306.h" // biblioteca do curso / driver do display
}

class DisplayDriver : public IRenderer {
public:
    void init() {
        i2c_init(i2c1, 400 * 1000);
        gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
        gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
        gpio_pull_up(PIN_I2C_SDA);
        gpio_pull_up(PIN_I2C_SCL);

        ssd1306_init(i2c1, OLED_I2C_ADDR, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    void beginFrame() { ssd1306_clear(); }
    void endFrame() { ssd1306_show(); }

    void drawRect(int x, int y, int w, int h, bool on = true) override {
        for (int dx = 0; dx < w; ++dx)
            for (int dy = 0; dy < h; ++dy)
                ssd1306_draw_pixel(x + dx, y + dy, on);
    }

    void drawFilledCircle(int cx, int cy, int r, bool on = true) override {
        for (int dx = -r; dx <= r; ++dx)
            for (int dy = -r; dy <= r; ++dy)
                if (dx * dx + dy * dy <= r * r)
                    ssd1306_draw_pixel(cx + dx, cy + dy, on);
    }
};
