#include "pico/stdlib.h"
#include "pico/time.h"

#include "entities/Ball.h"
#include "entities/Paddle.h"
#include "entities/HumanController.h"
#include "entities/AiController.h"
#include "game/GameManager.h"
#include "game/DifficultyMenu.h"
#include "hal/DisplayDriver.h"
#include "hal/JoystickInput.h"
#include "hal/ButtonInput.h"
#include "hal/BuzzerOutput.h"
#include "hal/RgbLedOutput.h"
#include "hal/NeoMatrixOutput.h"
#include "hal/CelebrationEffects.h"
#include "../include/PinConfig.h"

// ============================================================
// Game loop clássico, com passo de tempo fixo para física estável
// (comum em jogos, evita que a bola "voe" se um frame demorar mais).
// ============================================================

int main() {
    stdio_init_all();

    // --- Inicialização do hardware (HAL) ---
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

    // --- Tela de seleção de dificuldade (bloqueante, roda uma única vez) ---
    Difficulty chosenDifficulty = runDifficultyMenu(joystick, buttonA);
    DifficultyParams difficultyParams = getDifficultyParams(chosenDifficulty);

    // --- Criação das entidades ---
    HumanController humanController(&joystick);
    AiController aiController(difficultyParams.aiDeadZone, difficultyParams.aiResponsiveness);

    const float paddleWidth = 4.0f, paddleHeight = 16.0f;
    const float playerSpeed = 90.0f;

    Paddle leftPaddle(6.0f, SCREEN_HEIGHT / 2.0f, paddleWidth, paddleHeight,
                       playerSpeed, &humanController, SCREEN_HEIGHT);

    Paddle rightPaddle(SCREEN_WIDTH - 6.0f, SCREEN_HEIGHT / 2.0f, paddleWidth,
                        paddleHeight, difficultyParams.aiSpeed, &aiController, SCREEN_HEIGHT);

    Ball ball(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 2.5f, 60.0f);

    GameManager game(ball, leftPaddle, rightPaddle, SCREEN_WIDTH, SCREEN_HEIGHT);

    // CelebrationEffects se registra como observador — o GameManager não
    // sabe (e não precisa saber) que ela existe; só conhece
    // IGameEventListener (padrão Observer). Dá pra registrar mais de um
    // observador aqui no futuro (ex.: um que só loga eventos por USB).
    game.addListener(&effects);

    // --- Game loop com passo de tempo fixo ---
    constexpr float FIXED_DT = 1.0f / 60.0f; // 60 "updates" lógicos por segundo
    absolute_time_t lastTime = get_absolute_time();
    float accumulator = 0.0f;

    while (true) {
        absolute_time_t now = get_absolute_time();
        float frameTime = absolute_time_diff_us(lastTime, now) / 1'000'000.0f;
        lastTime = now;

        // Evita "espiral da morte" se algo travar momentaneamente
        if (frameTime > 0.25f) frameTime = 0.25f;
        accumulator += frameTime;

        // Passo lógico fixo: update de todas as entidades (via GameManager)
        while (accumulator >= FIXED_DT) {
            game.update(FIXED_DT);
            accumulator -= FIXED_DT;
        }

        // Renderização (pode rodar em taxa diferente do update lógico)
        display.beginFrame();
        game.draw(display);
        display.endFrame();

        sleep_ms(16); // ~60 fps de tela; ajuste conforme desempenho real
    }
}
