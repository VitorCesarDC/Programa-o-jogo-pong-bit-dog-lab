#pragma once
#include "BuzzerOutput.h"
#include "RgbLedOutput.h"
#include "NeoMatrixOutput.h"
#include "../game/GameEvent.h"
#include "../game/IGameEventListener.h"

// Concentra TODOS os efeitos de LED/buzzer/matriz num só lugar, e é o
// ÚNICO ponto do projeto que conhece frequências, cores e PWM. Implementa
// IGameEventListener para poder se registrar no GameManager como um
// observador comum — o GameManager não sabe (e não precisa saber) que
// esta classe existe; só conhece a interface.
class CelebrationEffects : public IGameEventListener {
public:
    CelebrationEffects(BuzzerOutput& buzzer, RgbLedOutput& led, NeoMatrixOutput& matrix)
        : buzzer_(buzzer), led_(led), matrix_(matrix) {}

    void onGameEvent(GameEvent event) override;

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
