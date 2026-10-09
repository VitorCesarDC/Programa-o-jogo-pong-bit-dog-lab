#pragma once

// ============================================================
// Pinagem da BitDogLab v6.3 (Raspberry Pi Pico / RP2040)
// Ajuste aqui se o seu diagrama for diferente.
// ============================================================

// LED RGB (catodo comum)
constexpr uint PIN_LED_R = 13;
constexpr uint PIN_LED_G = 11;
constexpr uint PIN_LED_B = 12;

// Botões (pull-up interno, pressionado = nível baixo)
constexpr uint PIN_BTN_A = 5;
constexpr uint PIN_BTN_B = 6;

// Buzzer passivo (PWM)
constexpr uint PIN_BUZZER = 21;

// Matriz de LEDs WS2812B 5x5 (não usada no Pong, deixada para efeitos futuros)
constexpr uint PIN_NEOPIXEL = 7;

// Joystick analógico
constexpr uint PIN_JOY_X = 27; // ADC1
constexpr uint PIN_JOY_Y = 26; // ADC0
constexpr uint PIN_JOY_SW = 22; // botão do joystick (pull-up)

// Display OLED (I2C na BitDogLab: GPIO 14 = SDA, GPIO 15 = SCL)
constexpr uint PIN_I2C_SDA = 14;
constexpr uint PIN_I2C_SCL = 15;
constexpr uint8_t OLED_I2C_ADDR = 0x3C;

// Resolução típica do display usado na BitDogLab
constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
