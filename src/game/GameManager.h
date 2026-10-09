#pragma once
#include "GameState.h"
#include "../entities/Ball.h"
#include "../entities/Paddle.h"
#include "../entities/Entity.h"
#include "../hal/CelebrationEffects.h"

// GameManager é o "terceiro" (pergunta 5): detecta colisões entre Ball e
// Paddle, controla placar e estado, e decide quando disparar som/luz.
// Nem a Ball nem a Paddle precisam se conhecer diretamente.
//
// Convenção: leftPaddle é sempre o JOGADOR (humano), rightPaddle é sempre
// o ADVERSÁRIO (IA). GoalFor / HatTrickFor / Victory sempre se referem ao
// jogador; GoalAgainst / HatTrickAgainst / Defeat, ao adversário.
class GameManager {
public:
    GameManager(Ball& ball, Paddle& leftPaddle, Paddle& rightPaddle,
                CelebrationEffects& effects,
                float screenWidth, float screenHeight);

    void update(float dt);
    void draw(IRenderer& renderer) const;

    GameState getState() const { return state_; }
    int getScoreLeft() const { return scoreLeft_; }
    int getScoreRight() const { return scoreRight_; }

private:
    void checkCollisions();
    void checkScoring();
    void registerGoal(bool playerScored);
    void startNewRound(int direction);

    Ball& ball_;
    Paddle& leftPaddle_;
    Paddle& rightPaddle_;
    CelebrationEffects& effects_;

    GameState state_ = GameState::Playing;
    int scoreLeft_ = 0;
    int scoreRight_ = 0;
    float screenWidth_;
    float screenHeight_;

    // Controle de sequência para detectar hat-trick (pergunta do usuário:
    // 3 gols SEGUIDOS do mesmo lado, sem o outro marcar entre eles).
    int lastScorerSide_ = 0; // 0 = ninguém ainda, 1 = jogador, -1 = adversário
    int streak_ = 0;

    static constexpr int WINNING_SCORE = 5;
    static constexpr int HAT_TRICK_STREAK = 3;
};
