#pragma once
#include "GameState.h"
#include "GameEvent.h"
#include "IGameEventListener.h"
#include "../entities/Ball.h"
#include "../entities/Paddle.h"
#include "../entities/Entity.h"

// GameManager é o "terceiro" (mediador): detecta colisões entre Ball e
// Paddle, controla placar e estado, e PUBLICA eventos para quem quiser
// escutar (padrão Observer). Não conhece buzzer, LED, matriz ou qualquer
// outro detalhe de hardware — só a interface IGameEventListener. Nem a
// Ball nem a Paddle precisam se conhecer diretamente.
//
// Convenção: leftPaddle é sempre o JOGADOR (humano), rightPaddle é sempre
// o ADVERSÁRIO (IA). GoalFor / HatTrickFor / Victory sempre se referem ao
// jogador; GoalAgainst / HatTrickAgainst / Defeat, ao adversário.
class GameManager {
public:
    GameManager(Ball& ball, Paddle& leftPaddle, Paddle& rightPaddle,
                float screenWidth, float screenHeight);

    // Registra um observador que será chamado a cada evento de jogo.
    // Até MAX_LISTENERS observadores, num array fixo — sem alocação
    // dinâmica, adequado a um microcontrolador.
    void addListener(IGameEventListener* listener);

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
    void notify(GameEvent event);

    Ball& ball_;
    Paddle& leftPaddle_;
    Paddle& rightPaddle_;

    static constexpr int MAX_LISTENERS = 4;
    IGameEventListener* listeners_[MAX_LISTENERS] = {nullptr, nullptr, nullptr, nullptr};
    int listenerCount_ = 0;

    GameState state_ = GameState::Playing;
    int scoreLeft_ = 0;
    int scoreRight_ = 0;
    float screenWidth_;
    float screenHeight_;

    // Pausa curta e NÃO bloqueante após um gol: dá tempo de ver o placar
    // mudar antes da bola voltar a se mover. Controlada por dt, igual ao
    // resto da física — não usa sleep_ms, então não existe dependência de
    // hardware aqui (totalmente testável fora da placa).
    float pointPauseRemaining_ = 0.0f;
    static constexpr float POINT_PAUSE_SECONDS = 0.5f;

    // Controle de sequência para detectar hat-trick (3 gols SEGUIDOS do
    // mesmo lado, sem o outro marcar entre eles).
    int lastScorerSide_ = 0; // 0 = ninguém ainda, 1 = jogador, -1 = adversário
    int streak_ = 0;

    static constexpr int WINNING_SCORE = 5;
    static constexpr int HAT_TRICK_STREAK = 3;
};
