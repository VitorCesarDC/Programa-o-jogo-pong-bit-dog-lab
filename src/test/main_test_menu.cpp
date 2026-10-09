// ============================================================
// Menu de teste sintético — dispara cada efeito de LED/buzzer/matriz
// isoladamente, pra validar o hardware ANTES de integrar no jogo.
//
// Navegação: joystick pra cima/baixo move a seleção; botão A dispara
// o efeito selecionado. Usa exatamente a mesma CelebrationEffects do
// jogo real — nada duplicado.
// ============================================================

#include "pico/stdlib.h"
#include "hal/DisplayDriver.h"
#include "hal/JoystickInput.h"
#include "hal/ButtonInput.h"
#include "hal/BuzzerOutput.h"
#include "hal/RgbLedOutput.h"
#include "hal/NeoMatrixOutput.h"
#include "hal/CelebrationEffects.h"
#include <cstdio>
#include "../include/PinConfig.h"

struct MenuItem {
    const char* label;
    GameEvent event;
};

static const MenuItem MENU_ITEMS[] = {
    {"Colisao",         GameEvent::Collision},
    {"Gol Jogador",     GameEvent::GoalFor},
    {"Gol Rival",       GameEvent::GoalAgainst},
    {"Hat-Trick Voce",  GameEvent::HatTrickFor},
    {"Hat-Trick Rival", GameEvent::HatTrickAgainst},
    {"Vitoria",         GameEvent::Victory},
    {"Derrota",         GameEvent::Defeat},
};
static constexpr int MENU_COUNT = sizeof(MENU_ITEMS) / sizeof(MenuItem);

extern "C" {
    #include "ssd1306.h"
}

static void drawMenu(int selectedIndex, bool justTriggered) {
    ssd1306_clear();

    // Cabeçalho
    ssd1306_draw_string(0, 0, "TESTE DE EFEITOS");
    ssd1306_draw_string(0, 10, "Joy:Nav  BtnA:Tocar");

    // Linha divisória horizontal
    for (int x = 0; x < 128; ++x) {
        ssd1306_draw_pixel(x, 19, true);
    }

    // Janela deslizante de 4 itens para caber na tela de 64px
    int startIdx = 0;
    if (selectedIndex >= 3) {
        startIdx = selectedIndex - 2;
        if (startIdx + 4 > MENU_COUNT) {
            startIdx = MENU_COUNT - 4;
        }
    }
    if (startIdx < 0) startIdx = 0;

    for (int k = 0; k < 4 && (startIdx + k) < MENU_COUNT; ++k) {
        int i = startIdx + k;
        int y = 23 + k * 10;
        char line[20];
        if (i == selectedIndex) {
            if (justTriggered) {
                snprintf(line, sizeof(line), "> %s *", MENU_ITEMS[i].label);
            } else {
                snprintf(line, sizeof(line), "> %s", MENU_ITEMS[i].label);
            }
        } else {
            snprintf(line, sizeof(line), "  %s", MENU_ITEMS[i].label);
        }
        ssd1306_draw_string(0, y, line);
    }

    ssd1306_show();
}

int main() {
    stdio_init_all();

    DisplayDriver display;
    JoystickInput joystick;
    ButtonInput buttonA(PIN_BTN_A);
    BuzzerOutput buzzer;
    RgbLedOutput led;
    NeoMatrixOutput matrix;

    display.init();
    joystick.init();
    buttonA.init();
    buzzer.init();
    led.init();
    matrix.init();

    CelebrationEffects effects(buzzer, led, matrix);

    int selected = 0;
    bool justTriggered = false;

    drawMenu(selected, false);

    while (true) {
        int step = joystick.readMenuStep();
        if (step != 0) {
            selected = (selected + step + MENU_COUNT) % MENU_COUNT;
            justTriggered = false;
            drawMenu(selected, justTriggered);
        }

        if (buttonA.wasPressed()) {
            justTriggered = true;
            drawMenu(selected, justTriggered); // feedback imediato no display

            // Dispara o efeito de verdade — LED, buzzer e/ou matriz,
            // dependendo do evento escolhido.
            effects.trigger(MENU_ITEMS[selected].event);

            justTriggered = false;
            drawMenu(selected, justTriggered);
        }

        sleep_ms(20); // taxa de leitura do menu, não precisa ser 60fps
    }
}
