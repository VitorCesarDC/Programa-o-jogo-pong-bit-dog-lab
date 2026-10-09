#pragma once
#include "BuzzerOutput.h"
#include "RgbLedOutput.h"
#include "NeoMatrixOutput.h"

// Enum de todos os eventos que têm efeito sonoro/visual associado.
// Usado tanto pelo GameManager real quanto pelo menu de teste — a MESMA
// função dispara o efeito nos dois lugares (nada duplicado).
enum class GameEvent {
    Collision,        // bola bate em parede/raquete
    GoalFor,           // jogador marca
    GoalAgainst,        // adversário marca
    HatTrickFor,        // jogador faz 3 seguidos
    HatTrickAgainst,     // adversário faz 3 seguidos
    Victory,
    Defeat
};

// Concentra TODOS os efeitos de LED/buzzer/matriz num só lugar. Nem o
// GameManager nem o menu de teste precisam saber frequências, cores ou
// PWM — só chamam trigger(evento).
class CelebrationEffects {
public:
    CelebrationEffects(BuzzerOutput& buzzer, RgbLedOutput& led, NeoMatrixOutput& matrix)
        : buzzer_(buzzer), led_(led), matrix_(matrix) {}

    void trigger(GameEvent event);

private:
    void playCollision();
    void playGoalFor();
    void playGoalAgainst();
    void playHatTrickFor();
    void playHatTrickAgainst();
    void playVictory();
    void playDefeat();

    void flashLed(uint8_t r, uint8_t g, uint8_t b, int times, int onMs, int offMs);

    BuzzerOutput& buzzer_;
    RgbLedOutput& led_;
    NeoMatrixOutput& matrix_;
};
