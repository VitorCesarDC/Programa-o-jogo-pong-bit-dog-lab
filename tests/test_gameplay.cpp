// ============================================================
// Testes de lógica de jogo SEM hardware — compila com g++ puro, sem
// Pico SDK. Cobre Ball, Paddle (com um controlador fixo) e GameManager
// (colisões, pontuação, hat-trick, pausa pós-gol, fim de jogo).
//
// Compilar e rodar (da raiz do projeto):
//   g++ -std=c++17 -I src tests/test_gameplay.cpp src/entities/Ball.cpp src/entities/Paddle.cpp src/game/GameManager.cpp -o test_gameplay
//   ./test_gameplay
// ============================================================

#include <cstdio>
#include <cmath>
#include <vector>

#include "entities/Ball.h"
#include "entities/Paddle.h"
#include "entities/IPaddleController.h"
#include "game/GameManager.h"
#include "game/GameEvent.h"
#include "game/IGameEventListener.h"

namespace {

int g_checks = 0;
int g_failures = 0;

void check(bool condition, const char* description) {
    ++g_checks;
    if (!condition) {
        ++g_failures;
        std::printf("  FALHOU: %s\n", description);
    }
}

bool approxEqual(float a, float b, float epsilon = 0.001f) {
    return std::fabs(a - b) < epsilon;
}

// Controlador que nunca move a raquete — útil para testes determinísticos
// de colisão/pontuação, onde não queremos que a raquete "fuja" da bola.
class StationaryController : public IPaddleController {
public:
    float getVerticalInput(float, float) override { return 0.0f; }
};

// Observador que só registra os eventos recebidos, na ordem, para o teste
// poder checar "aconteceu X" sem precisar de buzzer/LED reais.
class RecordingListener : public IGameEventListener {
public:
    void onGameEvent(GameEvent event) override { events.push_back(event); }
    std::vector<GameEvent> events;
};

void testBallMovesLinearlyOverTime() {
    std::printf("Ball::update move em linha reta\n");
    Ball ball(10.0f, 10.0f, 2.0f, 60.0f); // vx = 60, vy = 0
    ball.update(0.5f); // meio segundo
    check(approxEqual(ball.getX(), 40.0f), "x avanca vx*dt (10 + 60*0.5 = 40)");
    check(approxEqual(ball.getY(), 10.0f), "y nao muda sem vy");
}

void testBallBouncesOffWallInvertsOnlyVy() {
    std::printf("Ball::bounceOffWallVertical inverte so vy\n");
    Ball ball(10.0f, 10.0f, 2.0f, 60.0f);
    float vxBefore = ball.getVx();
    ball.bounceOffWallVertical();
    check(approxEqual(ball.getVx(), vxBefore), "vx nao muda na colisao com parede");

    Ball ball2(10.0f, 10.0f, 2.0f, 60.0f);
    ball2.bounceOffPaddle(10.0f, 8.0f); // gera algum vy != 0
    float vyAfterPaddle = ball2.getVy();
    ball2.bounceOffWallVertical();
    check(approxEqual(ball2.getVy(), -vyAfterPaddle), "vy inverte de sinal");
}

void testBallBouncesOffPaddleInvertsVxAndKeepsReasonableSpeed() {
    std::printf("Ball::bounceOffPaddle inverte vx e limita velocidade\n");
    Ball ball(10.0f, 10.0f, 2.0f, 60.0f);
    ball.bounceOffPaddle(/*paddleCenterY=*/10.0f, /*paddleHalfHeight=*/8.0f);
    check(ball.getVx() < 0.0f, "vx inverte de sinal (estava positivo)");

    float speed = std::sqrt(ball.getVx() * ball.getVx() + ball.getVy() * ball.getVy());
    check(speed <= 60.0f * 1.8f + 0.01f, "velocidade respeita o teto (1.8x a base)");
    check(speed >= 60.0f - 0.01f, "velocidade nao cai abaixo da base numa unica rebatida");
}

void testGameManagerScoresWhenBallCrossesSides() {
    std::printf("GameManager marca ponto quando a bola sai pela lateral\n");
    StationaryController controller;
    Paddle left(6.0f, 32.0f, 4.0f, 16.0f, 0.0f, &controller, 64.0f);
    Paddle right(122.0f, 32.0f, 4.0f, 16.0f, 0.0f, &controller, 64.0f);
    Ball ball(1.0f, 32.0f, 2.5f, 60.0f);
    ball.reset(1.0f, 32.0f, -1); // manda a bola pra fora pela esquerda

    GameManager game(ball, left, right, 128.0f, 64.0f);
    RecordingListener listener;
    game.addListener(&listener);

    for (int i = 0; i < 5; ++i) game.update(1.0f / 60.0f);

    check(game.getScoreRight() == 1, "placar do adversario incrementa quando a bola sai a esquerda");
    check(game.getScoreLeft() == 0, "placar do jogador nao muda");
    check(!listener.events.empty() && listener.events.back() == GameEvent::GoalAgainst,
          "evento GoalAgainst e publicado para os observadores");
}

void testHatTrickFiresOnThirdConsecutiveGoalFromSameSide() {
    std::printf("Hat-trick dispara no 3o gol seguido do mesmo lado\n");
    StationaryController controller;
    Paddle left(6.0f, 32.0f, 4.0f, 16.0f, 0.0f, &controller, 64.0f);
    Paddle right(122.0f, 32.0f, 4.0f, 16.0f, 0.0f, &controller, 64.0f);
    Ball ball(1.0f, 32.0f, 2.5f, 60.0f);

    GameManager game(ball, left, right, 128.0f, 64.0f);
    RecordingListener listener;
    game.addListener(&listener);

    auto forceGoalAgainst = [&]() {
        ball.reset(1.0f, 32.0f, -1);
        for (int i = 0; i < 5; ++i) game.update(1.0f / 60.0f);
        for (int i = 0; i < 60; ++i) game.update(1.0f / 60.0f); // consome a pausa pos-gol
    };

    forceGoalAgainst();
    forceGoalAgainst();
    forceGoalAgainst(); // terceiro gol seguido do adversario

    check(game.getScoreRight() == 3, "3 gols do adversario registrados");

    bool sawPlainGoal = false, sawHatTrick = false;
    for (GameEvent e : listener.events) {
        if (e == GameEvent::GoalAgainst) sawPlainGoal = true;
        if (e == GameEvent::HatTrickAgainst) sawHatTrick = true;
    }
    check(sawPlainGoal, "os 2 primeiros gols disparam GoalAgainst");
    check(sawHatTrick, "o 3o gol seguido dispara HatTrickAgainst em vez de GoalAgainst");
}

void testPointScoredPauseIsNonBlockingAndTimed() {
    std::printf("Pausa pos-gol (PointScored) usa dt, nao bloqueia, e tem fim\n");
    StationaryController controller;
    Paddle left(6.0f, 32.0f, 4.0f, 16.0f, 0.0f, &controller, 64.0f);
    Paddle right(122.0f, 32.0f, 4.0f, 16.0f, 0.0f, &controller, 64.0f);
    Ball ball(1.0f, 32.0f, 2.5f, 60.0f);
    ball.reset(1.0f, 32.0f, -1);

    GameManager game(ball, left, right, 128.0f, 64.0f);

    for (int i = 0; i < 5; ++i) game.update(1.0f / 60.0f); // dispara o gol
    check(game.getState() == GameState::PointScored, "estado vira PointScored logo apos o gol");

    float xDuringPause = ball.getX();
    game.update(1.0f / 60.0f);
    check(approxEqual(ball.getX(), xDuringPause), "bola fica parada durante a pausa pos-gol");

    for (int i = 0; i < 60; ++i) game.update(1.0f / 60.0f); // passa da pausa de 0.5s
    check(game.getState() == GameState::Playing, "volta a Playing depois da pausa");
}

void testGameEndsAtWinningScoreAndStopsUpdating() {
    std::printf("Fim de jogo ao atingir o placar de vitoria\n");
    StationaryController controller;
    Paddle left(6.0f, 32.0f, 4.0f, 16.0f, 0.0f, &controller, 64.0f);
    Paddle right(122.0f, 32.0f, 4.0f, 16.0f, 0.0f, &controller, 64.0f);
    Ball ball(1.0f, 32.0f, 2.5f, 60.0f);

    GameManager game(ball, left, right, 128.0f, 64.0f);
    RecordingListener listener;
    game.addListener(&listener);

    auto forceGoalFor = [&]() {
        ball.reset(126.0f, 32.0f, 1);
        for (int i = 0; i < 5; ++i) game.update(1.0f / 60.0f);
        for (int i = 0; i < 60; ++i) game.update(1.0f / 60.0f);
    };

    for (int i = 0; i < 5; ++i) forceGoalFor(); // placar de vitoria = 5

    check(game.getScoreLeft() == 5, "jogador chega a 5 pontos");
    check(game.getState() == GameState::GameOver, "estado vira GameOver");

    bool sawVictory = false;
    for (GameEvent e : listener.events) if (e == GameEvent::Victory) sawVictory = true;
    check(sawVictory, "evento Victory e publicado ao vencer");

    float xBefore = ball.getX();
    game.update(1.0f / 60.0f);
    check(approxEqual(ball.getX(), xBefore), "nada mais se move depois de GameOver");
}

} // namespace

int main() {
    testBallMovesLinearlyOverTime();
    testBallBouncesOffWallInvertsOnlyVy();
    testBallBouncesOffPaddleInvertsVxAndKeepsReasonableSpeed();
    testGameManagerScoresWhenBallCrossesSides();
    testHatTrickFiresOnThirdConsecutiveGoalFromSameSide();
    testPointScoredPauseIsNonBlockingAndTimed();
    testGameEndsAtWinningScoreAndStopsUpdating();

    std::printf("\n%d checagens, %d falha(s)\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
