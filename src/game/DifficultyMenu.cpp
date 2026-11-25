#include "DifficultyMenu.h"
#include "pico/time.h"
#include <cstdio>

extern "C" {
    #include "ssd1306.h"
}

namespace {

struct MenuItem {
    Difficulty difficulty;
    const char* label;
};

constexpr MenuItem MENU_ITEMS[] = {
    {Difficulty::Easy,   "Facil"},
    {Difficulty::Medium, "Medio"},
    {Difficulty::Hard,   "Dificil"},
};
constexpr int MENU_COUNT = sizeof(MENU_ITEMS) / sizeof(MenuItem);

void drawDifficultyMenu(int selectedIndex) {
    ssd1306_clear();
    ssd1306_draw_string(0, 0, "ESCOLHA A");
    ssd1306_draw_string(0, 10, "DIFICULDADE");

    for (int x = 0; x < 128; ++x) {
        ssd1306_draw_pixel(x, 21, true);
    }

    for (int i = 0; i < MENU_COUNT; ++i) {
        int y = 26 + i * 10;
        char line[20];
        snprintf(line, sizeof(line), "%s %s",
                 (i == selectedIndex) ? ">" : " ", MENU_ITEMS[i].label);
        ssd1306_draw_string(0, y, line);
    }

    ssd1306_draw_string(0, 56, "Joy:Nav BtnA:OK");
    ssd1306_show();
}

} // namespace

DifficultyParams getDifficultyParams(Difficulty difficulty) {
    switch (difficulty) {
        case Difficulty::Easy:
            return {"Facil", 22.0f, 10.0f, 0.35f};
        case Difficulty::Hard:
            return {"Dificil", 50.0f, 3.0f, 0.85f};
        case Difficulty::Medium:
        default:
            return {"Medio", 34.0f, 5.0f, 0.60f};
    }
}

Difficulty runDifficultyMenu(JoystickInput& joystick, ButtonInput& buttonA) {
    int selected = 1; // começa em "Medio"
    drawDifficultyMenu(selected);

    while (true) {
        int step = joystick.readMenuStep();
        if (step != 0) {
            selected = (selected + step + MENU_COUNT) % MENU_COUNT;
            drawDifficultyMenu(selected);
        }

        if (buttonA.wasPressed()) {
            return MENU_ITEMS[selected].difficulty;
        }

        sleep_ms(20);
    }
}
