#pragma once
#include "../hal/JoystickInput.h"
#include "../hal/ButtonInput.h"

// Dificuldades disponíveis para o oponente (IA). Cada uma mapeia para um
// conjunto de parâmetros de AiController + velocidade da raquete direita.
enum class Difficulty {
    Easy,
    Medium,
    Hard
};

struct DifficultyParams {
    const char* label;
    float aiSpeed;          // pixels/s da raquete da IA
    float aiDeadZone;       // zona morta do AiController (pixels)
    float aiResponsiveness; // 0..1, quão rápido a IA corrige o erro
};

// Mapeia uma dificuldade para os parâmetros concretos usados ao montar o
// AiController e a Paddle da direita em main.cpp.
DifficultyParams getDifficultyParams(Difficulty difficulty);

// Tela de seleção de dificuldade, mostrada uma vez antes do jogo começar:
// joystick navega (readMenuStep), botão A confirma. Pressupõe que o
// display já foi inicializado (DisplayDriver::init() chamado em main.cpp);
// esta função só desenha nele via a lib ssd1306, igual ao menu de teste.
// Bloqueia até o jogador escolher — intencional, roda uma única vez antes
// do game loop, não faz parte dele.
Difficulty runDifficultyMenu(JoystickInput& joystick, ButtonInput& buttonA);
