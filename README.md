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
| Testes sem hardware | [`tests/`](tests/) |

## Estrutura do código

```
include/PinConfig.h               - pinagem da BitDogLab
src/entities/                     - entidades do jogo (Ball, Paddle) e a interface Entity
src/entities/IPaddleController.h  - estratégia de controle (Strategy pattern)
src/entities/HumanController.h    - controlador via joystick
src/entities/AiController.h       - controlador de IA
src/game/GameEvent.h              - enum de eventos de jogo (sem dependência de hardware)
src/game/IGameEventListener.h     - interface do Observer (quem escuta eventos de jogo)
src/game/GameManager.*            - orquestrador: colisões, placar, estado, game loop lógico
src/game/DifficultyMenu.*         - tela de seleção de dificuldade (Fácil/Médio/Difícil)
src/hal/                          - camada de hardware (display, joystick, buzzer, LED)
src/hal/CelebrationEffects.*      - observador concreto: traduz eventos em som/luz
src/main.cpp                      - game loop físico com passo de tempo fixo
src/test/main_test_menu.cpp       - menu de teste sintético dos efeitos
tests/test_gameplay.cpp           - testes de lógica SEM hardware (g++ puro)
docs/                             - documentação (SGDD, entidades, componentes)
```

## Decisões de arquitetura

- **Separação por entidades**: `Ball` e `Paddle` só cuidam de si mesmas (posição, movimento, reação a um evento avisado por fora). Nenhuma conhece a outra diretamente.
- **Mediador**: `GameManager` detecta colisões e decide o que fazer, evitando acoplamento direto entre entidades.
- **Strategy para controle**: `Paddle` é uma única classe usada pelo jogador humano e pela IA; muda apenas o `IPaddleController` injetado.
- **Observer para eventos de jogo**: `GameManager` não conhece buzzer, LED ou matriz — ele só conhece a interface `IGameEventListener` e publica eventos (`notify()`) para quem estiver registrado (`addListener()`). `CelebrationEffects` é um observador concreto entre vários possíveis; o `GameManager` nunca inclui nada de hardware (`GameManager.h` não depende de `CelebrationEffects.h`, `pwm.h`, `i2c.h` etc.).
- **Abstração de hardware**: as entidades desenham via `IRenderer`, nunca chamando a lib do display diretamente.
- **Game loop com passo fixo**: a lógica roda em incrementos fixos (`FIXED_DT`), desacoplada da renderização, para física estável.
- **Pausa pós-gol não bloqueante**: depois de um gol, o jogo entra no estado `PointScored` por `GameManager::POINT_PAUSE_SECONDS` (0.5s), controlado por `dt` — igual ao resto da física, sem `sleep_ms`. As raquetes continuam respondendo durante a pausa.

## Dois executáveis

- **`pong_test_menu`**: menu de teste isolado (joystick navega, botão A dispara). Serve para validar LED, buzzer e matriz na placa.
- **`pong_bitdoglab`**: o jogo completo. Ao ligar, mostra primeiro a tela de seleção de dificuldade (joystick navega, botão A confirma) antes de começar a partida.

Ambos compartilham `src/hal/CelebrationEffects.*`.

## Dificuldade do oponente

A tela de seleção (`src/game/DifficultyMenu.*`) ajusta três parâmetros do `AiController` e a velocidade da raquete da IA:

| Dificuldade | Velocidade da raquete | Zona morta | Responsividade |
|---|---|---|---|
| Fácil | 22 px/s | 10 px | 0.35 |
| Médio (padrão) | 34 px/s | 5 px | 0.60 |
| Difícil | 50 px/s | 3 px | 0.85 |

## Efeitos implementados (`src/hal/CelebrationEffects.cpp`)

| Evento               | LED                              | Som                             | Streak                         |
| -------------------- | -------------------------------- | -------------------------------- | ------------------------------ |
| Colisão              | nenhum                           | beep curto e suave (duty baixo)  | —                              |
| Gol do jogador       | azul                              | chirp ascendente curto           | —                              |
| Gol do adversário    | vermelho                          | chirp descendente curto          | —                              |
| Hat-trick jogador    | roxo piscando 3x                 | fanfarra ascendente de 4 notas   | 3 gols seguidos, mesmo lado    |
| Hat-trick adversário | laranja piscando 3x              | melodia descendente de 4 notas   | idem                           |
| Vitória              | matriz verde (varredura + flash) | melodia alegre de 5 notas        | placar do jogador chega a 5    |
| Derrota              | matriz vermelha "apagando"       | melodia triste de 4 notas        | placar do adversário chega a 5 |

A lógica de hat-trick fica em `GameManager::registerGoal()`: guarda o lado do último gol e um contador (`streak_`); zera quando o outro lado marca.

`CelebrationEffects` é chamada tanto pelo `GameManager` (via `notify()` → `IGameEventListener::onGameEvent()`) quanto pelo menu de teste (chamando `onGameEvent()` diretamente) — nunca duplicada.

**Observação sobre bloqueio**: os efeitos usam `sleep_ms` internamente (buzzer e flash de LED), então o jogo fica pausado durante a duração do efeito (até ~1s em vitória/derrota). Isso é intencional aqui, porque só acontece em momentos em que o rali já parou (gol, fim de jogo). Se um dia quiser os efeitos tocando sem travar o resto do jogo, a mudança necessária é trocar os `sleep_ms` de `BuzzerOutput`/`RgbLedOutput`/`NeoMatrixOutput` por uma máquina de estados baseada em tempo (como já é feito no debounce de `ButtonInput`), mas isso é um projeto de refatoração próprio.

## Jogabilidade

- A bola acelera ligeiramente a cada rebatida na raquete (`Ball::bounceOffPaddle`, fator `1.05x` por rebatida, até um teto de `1.8x` a velocidade base) e volta ao normal no início de cada rali (`Ball::reset`). Dá uma sensação de progressão dentro do ponto sem deixar o jogo impossível.

### Ideias para o futuro (não implementadas)

- Pausa manual (botão dedicado) durante `Playing`.
- Placar alvo configurável no mesmo menu de dificuldade.
- Som variando de tom conforme o ponto de impacto na raquete (hoje o som de colisão é sempre igual).
- "Ponto de ouro"/sudden death quando os dois lados estão perto do placar de vitória.
- Efeito visual na matriz 5x5 durante o rali (trilha da bola), não só nas comemorações.
- IA com dificuldade adaptativa dentro da própria partida (rubber-banding) em vez de fixa por partida.
- Modo 2 jogadores locais (segunda `Paddle` com `HumanController` ligado a outro botão/joystick, se a placa tiver).

## Testes sem hardware (`tests/test_gameplay.cpp`)

`Ball`, `Paddle` e `GameManager` não dependem de nenhum header de hardware (`machine`, `pwm`, `i2c`, `pio` etc. ficam isolados na camada `hal/` e em `CelebrationEffects`). Isso permite testar toda a lógica de jogo — movimento, colisões, pontuação, hat-trick, pausa pós-gol, fim de jogo — com `g++` comum, sem Pico SDK e sem a placa conectada.

Compilar e rodar (da raiz do projeto):

```bash
g++ -std=c++17 -I src tests/test_gameplay.cpp src/entities/Ball.cpp src/entities/Paddle.cpp src/game/GameManager.cpp -o test_gameplay
./test_gameplay
```

O teste usa um `RecordingListener` (implementa `IGameEventListener`) para verificar que os eventos certos (`GoalFor`, `HatTrickAgainst`, `Victory` etc.) são publicados nos momentos certos, sem precisar de buzzer ou LED reais.

## Como compilar o firmware

1. Configure o `pico-sdk` (ex.: variável `PICO_SDK_PATH`).
2. `mkdir build && cd build && cmake .. && make -j4`
3. Grave o `.uf2` gerado na placa (modo BOOTSEL).

O jogo foi testado e validado na placa BitDogLab real.
