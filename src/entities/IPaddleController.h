#pragma once

// Estratégia de controle de uma Paddle. Existe para que a MESMA classe
// Paddle sirva tanto para o jogador humano quanto para a IA, evitando
// duplicar comportamento idêntico em duas classes (pergunta 7).
class IPaddleController {
public:
    virtual ~IPaddleController() = default;

    // Retorna a velocidade vertical desejada (-1.0 a 1.0), normalizada.
    // ballY é passado pelo GameManager quando necessário (ex.: IA), em vez
    // da Paddle guardar um ponteiro direto para a Ball (pergunta 5).
    virtual float getVerticalInput(float ownY, float ballY) = 0;
};
