#pragma once

// Todos os eventos de jogo que algum observador (som, luz, display, log...)
// pode querer reagir. Fica num header próprio, sem nenhuma dependência de
// hardware, para que GameManager e qualquer observador possam incluí-lo
// sem arrastar PWM/I2C/PIO para dentro da lógica do jogo.
enum class GameEvent {
    Collision,        // bola bate em parede/raquete
    GoalFor,           // jogador marca
    GoalAgainst,        // adversário marca
    HatTrickFor,        // jogador faz 3 seguidos
    HatTrickAgainst,     // adversário faz 3 seguidos
    Victory,
    Defeat
};
