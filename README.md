# Pong — BitDogLab v6.3 (C++ / Pico SDK)

Jogo Pong em C++ para a placa **BitDogLab v6.3** (Raspberry Pi Pico), desenvolvido para a disciplina **SMD0053 – Programação para Jogos I** (2026.2).

**Projeto individual** — autor: VitorCesarDC

## Índice

| Parte | Onde está |
|---|---|
| 1. SGDD (Documento de Design do Jogo) | [`docs/sgdd/SGDD.md`](docs/sgdd/SGDD.md) |
| 2. Entidades do jogo | [`docs/entidades/entidades.md`](docs/entidades/entidades.md) |
| 3. Entidades e seus componentes | [`docs/componentes/componentes.md`](docs/componentes/componentes.md) |
| Código-fonte | [`src/`](src/) e [`include/`](include/) |

## Estrutura do código

```
include/PinConfig.h               - pinagem da BitDogLab
src/entities/                     - entidades do jogo (Ball, Paddle) e a interface Entity
src/entities/IPaddleController.h  - estratégia de controle (Strategy pattern)
src/entities/HumanController.h    - controlador via joystick
src/entities/AiController.h       - controlador de IA
src/game/GameManager.*            - orquestrador: colisões, placar, estado, game loop lógico
src/hal/                          - camada de hardware (display, joystick, buzzer, LED)
src/main.cpp                      - game loop físico com passo de tempo fixo
docs/                             - documentação (SGDD, entidades, componentes)
```

## Decisões de arquitetura

- **Separação por entidades**: `Ball` e `Paddle` só cuidam de si mesmas (posição, movimento, reação a um evento avisado por fora). Nenhuma conhece a outra diretamente.
- **Mediador**: `GameManager` detecta colisões e decide o que fazer, evitando acoplamento direto entre entidades.
- **Strategy para controle**: `Paddle` é uma única classe usada pelo jogador humano e pela IA; muda apenas o `IPaddleController` injetado.
- **Abstração de hardware**: as entidades desenham via `IRenderer`, nunca chamando a lib do display diretamente.
- **Game loop com passo fixo**: a lógica roda em incrementos fixos (`FIXED_DT`), desacoplada da renderização, para física estável.

## Dois executáveis

- **`pong_test_menu`**: menu de teste isolado (joystick navega, botão A dispara). Serve para validar LED, buzzer e matriz na placa.
- **`pong_bitdoglab`**: o jogo completo.

Ambos compartilham `src/hal/CelebrationEffects.*`.

## Efeitos implementados (`src/hal/CelebrationEffects.cpp`)

| Evento               | LED                              | Som                             | Streak                         |
| -------------------- | -------------------------------- | ------------------------------- | ------------------------------ |
| Colisão              | nenhum                           | beep curto e suave (duty baixo) | —                              |
| Gol do jogador       | azul                             | chirp ascendente curto          | —                              |
| Gol do adversário    | vermelho                         | chirp descendente curto         | —                              |
| Hat-trick jogador    | roxo piscando 3x                 | fanfarra ascendente de 4 notas  | 3 gols seguidos, mesmo lado    |
| Hat-trick adversário | laranja piscando 3x              | melodia descendente de 4 notas  | idem                           |
| Vitória              | matriz verde (varredura + flash) | melodia alegre de 5 notas       | placar do jogador chega a 5    |
| Derrota              | matriz vermelha "apagando"       | melodia triste de 4 notas       | placar do adversário chega a 5 |

A lógica de hat-trick fica em `GameManager::registerGoal()`: guarda o lado do último gol e um contador (`streak_`); zera quando o outro lado marca.

## Como compilar

1. Configure o `pico-sdk` (ex.: variável `PICO_SDK_PATH`).
2. `mkdir build && cd build && cmake .. && make -j4`
3. Grave o `.uf2` gerado na placa (modo BOOTSEL).

O jogo foi testado e validado na placa BitDogLab real.
