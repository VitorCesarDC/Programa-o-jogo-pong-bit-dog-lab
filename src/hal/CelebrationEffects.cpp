#include "CelebrationEffects.h"

// Notas musicais (Hz) usadas nas melodias, pra ficar legível.
namespace Notes {
    constexpr uint32_t C4 = 262, D4 = 294, E4 = 330, F4 = 349, G4 = 392, A4 = 440, B4 = 494;
    constexpr uint32_t C5 = 523, D5 = 587, E5 = 659, F5 = 698, G5 = 784, A5 = 880;
    constexpr uint32_t C6 = 1047;
    constexpr uint32_t REST = 0;
}
using namespace Notes;

void CelebrationEffects::trigger(GameEvent event) {
    switch (event) {
        case GameEvent::Collision:       playCollision();      break;
        case GameEvent::GoalFor:         playGoalFor();        break;
        case GameEvent::GoalAgainst:     playGoalAgainst();    break;
        case GameEvent::HatTrickFor:     playHatTrickFor();    break;
        case GameEvent::HatTrickAgainst: playHatTrickAgainst();break;
        case GameEvent::Victory:         playVictory();        break;
        case GameEvent::Defeat:          playDefeat();         break;
    }
}

void CelebrationEffects::flashLed(uint8_t r, uint8_t g, uint8_t b, int times, int onMs, int offMs) {
    for (int i = 0; i < times; ++i) {
        led_.setColor(r, g, b);
        sleep_ms(onMs);
        led_.setColor(0, 0, 0);
        if (i < times - 1) sleep_ms(offMs);
    }
}

// --- Colisão: som curto e suave, sem LED (evento mais frequente do jogo,
// não pode cansar o ouvido nem distrair visualmente). ---
void CelebrationEffects::playCollision() {
    buzzer_.playSoftBeep(900, 15);
}

// --- Gol a favor: LED azul + chirp curto ascendente ---
void CelebrationEffects::playGoalFor() {
    led_.setColor(0, 80, 255); // azul
    static const Note melody[] = { {E5, 60}, {G5, 90} };
    buzzer_.playMelody(melody, 2);
    sleep_ms(120);
    led_.setColor(0, 0, 0);
}

// --- Gol contra: LED vermelho + som descendente (claramente diferente) ---
void CelebrationEffects::playGoalAgainst() {
    led_.setColor(255, 20, 0); // vermelho
    static const Note melody[] = { {A4, 60}, {F4, 100} };
    buzzer_.playMelody(melody, 2);
    sleep_ms(120);
    led_.setColor(0, 0, 0);
}

// --- Hat-trick do jogador: cor roxa/magenta piscando + fanfarra ascendente ---
void CelebrationEffects::playHatTrickFor() {
    static const Note melody[] = { {C5, 80}, {E5, 80}, {G5, 80}, {C6, 160} };
    buzzer_.playMelody(melody, 4);
    flashLed(200, 0, 255, 3, 90, 80); // roxo piscando 3x
}

// --- Hat-trick do adversário: cor laranja piscando + melodia descendente "ameaçadora" ---
void CelebrationEffects::playHatTrickAgainst() {
    static const Note melody[] = { {C5, 80}, {A4, 80}, {F4, 80}, {D4, 160} };
    buzzer_.playMelody(melody, 4);
    flashLed(255, 110, 0, 3, 90, 80); // laranja piscando 3x
}

// --- Vitória: varredura verde na matriz sincronizada com melodia alegre ---
void CelebrationEffects::playVictory() {
    static const Note melody[] = {
        {C5, 100}, {E5, 100}, {G5, 100}, {C6, 100}, {G5, 150}
    };
    matrix_.clear();
    matrix_.show();

    for (int col = 0; col < NeoMatrixOutput::WIDTH; ++col) {
        for (int row = 0; row < NeoMatrixOutput::HEIGHT; ++row) {
            matrix_.setPixel(col, row, 0, 200, 40); // verde
        }
        matrix_.show();
        buzzer_.playBeep(melody[col].freqHz, melody[col].durationMs);
    }

    // flash final de vitória
    for (int i = 0; i < 3; ++i) {
        matrix_.fill(0, 255, 60);
        matrix_.show();
        sleep_ms(120);
        matrix_.clear();
        matrix_.show();
        sleep_ms(100);
    }
    buzzer_.playBeep(melody[4].freqHz, melody[4].durationMs);
}

// --- Derrota: varredura vermelha "apagando" na matriz + melodia triste ---
void CelebrationEffects::playDefeat() {
    static const Note melody[] = {
        {G4, 120}, {F4, 120}, {D4, 120}, {C4, 260}
    };

    matrix_.fill(160, 0, 0); // vermelho
    matrix_.show();

    for (int col = NeoMatrixOutput::WIDTH - 1; col >= 0; --col) {
        for (int row = 0; row < NeoMatrixOutput::HEIGHT; ++row) {
            matrix_.setPixel(col, row, 0, 0, 0); // apaga coluna a coluna
        }
        matrix_.show();
        if (col < 4) buzzer_.playBeep(melody[3 - col].freqHz, melody[3 - col].durationMs);
        else sleep_ms(80);
    }

    sleep_ms(100);
    matrix_.clear();
    matrix_.show();
}
