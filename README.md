# Pong — BitDogLab v6.3 (C++ / Pico SDK)

## Estrutura

```
include/PinConfig.h        - pinagem da BitDogLab
src/entities/               - entidades do jogo (Ball, Paddle) e a interface Entity
src/entities/IPaddleController.h  - estratégia de controle (Strategy pattern)
src/entities/HumanController.h    - controlador via joystick
src/entities/AiController.h       - controlador de IA
src/game/GameManager.*      - orquestrador: colisões, placar, estado, game loop lógico
src/hal/                    - camada de hardware (display, joystick, buzzer, LED)
src/main.cpp                - game loop físico com passo de tempo fixo
```

## Decisões de arquitetura (resumo para a apresentação)

- **Separação por entidades**: `Ball` e `Paddle` só sabem cuidar de si mesmas
  (posição, movimento, reação a um evento que alguém de fora avisa). Nenhuma
  conhece a outra diretamente.
- **Mediador**: `GameManager` é quem detecta colisões e decide o que fazer —
  evita acoplamento direto entre entidades.
- **Strategy para controle**: `Paddle` é uma única classe reutilizada para
  jogador humano e IA, trocando apenas o `IPaddleController` injetado.
- **Abstração de hardware**: as entidades desenham via `IRenderer`, nunca
  chamando a lib do display diretamente — troque o driver sem tocar na
  lógica do jogo.
- **Game loop com passo fixo**: a lógica roda em incrementos fixos de tempo
  (`FIXED_DT`), desacoplada da taxa de renderização, para física estável.

## Dois executáveis

- **`pong_test_menu`**: menu de teste isolado (joystick navega, botão A
  dispara). Use este primeiro pra validar LED, buzzer e matriz na placa
  antes de tocar no jogo.
- **`pong_bitdoglab`**: o jogo completo, já usando os mesmos efeitos.

Ambos compartilham `src/hal/CelebrationEffects.*` — o efeito é definido
uma única vez e usado nos dois lugares.

## Efeitos implementados (`src/hal/CelebrationEffects.cpp`)

| Evento | LED | Som | Streak |
|---|---|---|---|
| Colisão | nenhum | beep curto e suave (duty baixo) | — |
| Gol do jogador | azul | chirp ascendente curto | — |
| Gol do adversário | vermelho | chirp descendente curto | — |
| Hat-trick jogador | roxo piscando 3x | fanfarra ascendente de 4 notas | 3 gols seguidos, mesmo lado |
| Hat-trick adversário | laranja piscando 3x | melodia descendente de 4 notas | idem |
| Vitória | matriz verde (varredura + flash) | melodia alegre de 5 notas | placar do jogador chega a 5 |
| Derrota | matriz vermelha "apagando" | melodia triste de 4 notas | placar do adversário chega a 5 |

A lógica de hat-trick fica no `GameManager::registerGoal()`: guarda o lado
do último gol e um contador (`streak_`); zera quando o outro lado marca.

## Antes de compilar

1. Configure o `pico-sdk` (ex.: `PICO_SDK_PATH`) e copie o
   `pico_sdk_import.cmake` do SDK para a raiz do projeto.
2. Ajuste `src/hal/DisplayDriver.h` para bater com a lib de display (ssd1306
   ou similar) fornecida na disciplina — os nomes de função usados aqui
   (`ssd1306_init`, `ssd1306_draw_pixel`, etc.) são um placeholder e
   provavelmente precisam ser trocados pelos nomes reais da lib do curso.
3. Confira a pinagem em `include/PinConfig.h` contra o diagrama exato da
   sua placa — pode variar entre revisões da v6.3.
4. `mkdir build && cd build && cmake .. && make -j4`

Este esqueleto não foi compilado/testado em hardware real (ambiente sem
acesso à internet/SDK aqui) — revise principalmente `BuzzerOutput` (cálculo
de PWM) e `DisplayDriver` antes de gravar na placa.
