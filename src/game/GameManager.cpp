#include "GameManager.h"
#include <cmath>

GameManager::GameManager(Ball& ball, Paddle& leftPaddle, Paddle& rightPaddle,
                          CelebrationEffects& effects,
                          float screenWidth, float screenHeight)
    : ball_(ball), leftPaddle_(leftPaddle), rightPaddle_(rightPaddle),
      effects_(effects),
      screenWidth_(screenWidth), screenHeight_(screenHeight) {}

void GameManager::update(float dt) {
    if (state_ != GameState::Playing) return;

    ball_.update(dt);
    leftPaddle_.update(dt, ball_.getY());
    rightPaddle_.update(dt, ball_.getY());

    // Colisão com topo/base — mesma reação nos dois lados, som suave.
    if ((ball_.getY() - ball_.getRadius() <= 0 && ball_.getVy() < 0) ||
        (ball_.getY() + ball_.getRadius() >= screenHeight_ && ball_.getVy() > 0)) {
        ball_.bounceOffWallVertical();
        effects_.trigger(GameEvent::Collision);
    }

    checkCollisions();
    checkScoring();
}

void GameManager::checkCollisions() {
    auto collidesWith = [&](const Paddle& p) {
        bool overlapX = std::fabs(ball_.getX() - p.getX()) <= (p.getWidth() / 2.0f + ball_.getRadius());
        bool overlapY = std::fabs(ball_.getY() - p.getY()) <= (p.getHeight() / 2.0f + ball_.getRadius());
        return overlapX && overlapY;
    };

    if (collidesWith(leftPaddle_) && ball_.getVx() < 0) {
        ball_.bounceOffPaddle(leftPaddle_.getY(), leftPaddle_.getHeight() / 2.0f);
        effects_.trigger(GameEvent::Collision); // som suave, igual ao de parede
    } else if (collidesWith(rightPaddle_) && ball_.getVx() > 0) {
        ball_.bounceOffPaddle(rightPaddle_.getY(), rightPaddle_.getHeight() / 2.0f);
        effects_.trigger(GameEvent::Collision);
    }
}

void GameManager::checkScoring() {
    // Sair pela lateral NÃO é colisão — é evento de pontuação, com reação
    // própria (diferente de um "quique").
    if (ball_.getX() < 0) {
        scoreRight_++;
        registerGoal(false); // adversário marcou
        startNewRound(1);
    } else if (ball_.getX() > screenWidth_) {
        scoreLeft_++;
        registerGoal(true); // jogador marcou
        startNewRound(-1);
    }

    if (scoreLeft_ >= WINNING_SCORE) {
        state_ = GameState::GameOver;
        effects_.trigger(GameEvent::Victory);
    } else if (scoreRight_ >= WINNING_SCORE) {
        state_ = GameState::GameOver;
        effects_.trigger(GameEvent::Defeat);
    }
}

void GameManager::registerGoal(bool playerScored) {
    int side = playerScored ? 1 : -1;

    // Atualiza a sequência (streak) de gols consecutivos do mesmo lado.
    if (side == lastScorerSide_) {
        streak_++;
    } else {
        lastScorerSide_ = side;
        streak_ = 1;
    }

    bool isHatTrick = (streak_ == HAT_TRICK_STREAK);

    if (playerScored) {
        effects_.trigger(isHatTrick ? GameEvent::HatTrickFor : GameEvent::GoalFor);
    } else {
        effects_.trigger(isHatTrick ? GameEvent::HatTrickAgainst : GameEvent::GoalAgainst);
    }

    // Depois do hat-trick, a contagem reinicia — o próximo gol do mesmo
    // lado começa uma sequência nova (evita repetir o efeito de hat-trick
    // a cada gol subsequente). Ajuste aqui se quiser outro comportamento.
    if (isHatTrick) {
        streak_ = 0;
        lastScorerSide_ = 0;
    }
}

void GameManager::startNewRound(int direction) {
    ball_.reset(screenWidth_ / 2.0f, screenHeight_ / 2.0f, direction);
}

void GameManager::draw(IRenderer& renderer) const {
    // Linha central pontilhada (rede)
    for (int y = 0; y < static_cast<int>(screenHeight_); y += 6) {
        renderer.drawRect(static_cast<int>(screenWidth_ / 2.0f), y, 1, 3);
    }

    // Placar simples: marcadores de pontos no topo
    for (int i = 0; i < scoreLeft_; ++i) {
        renderer.drawRect(static_cast<int>(screenWidth_ / 2.0f) - 10 - (i * 6), 2, 4, 3);
    }
    for (int i = 0; i < scoreRight_; ++i) {
        renderer.drawRect(static_cast<int>(screenWidth_ / 2.0f) + 6 + (i * 6), 2, 4, 3);
    }

    ball_.draw(renderer);
    leftPaddle_.draw(renderer);
    rightPaddle_.draw(renderer);
}
