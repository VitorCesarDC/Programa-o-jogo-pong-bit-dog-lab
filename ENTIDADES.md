# Entidades do jogo (revisado)

Projeto individual — VitorCesarDC

## Lista de entidades

| Entidade | Tipo | Descrição |
|---|---|---|
| `Ball` | Entidade de jogo | A bola. Move-se a cada quadro e muda de direção ao colidir. |
| `Paddle` (esquerda) | Entidade de jogo | Raquete do jogador, controlada pelo joystick. |
| `Paddle` (direita) | Entidade de jogo | Raquete do adversário, controlada pela IA. Mesma classe da esquerda. |
| `GameManager` | Sistema / mediador | Não é uma entidade visível; detecta colisões, controla placar e estados. |
| Placar / HUD | Dado | Guardado no `GameManager` e desenhado pelo renderer; não tem posição nem `update()` próprio. |
| Buzzer e LED RGB / matriz | Atuadores | Acionados por eventos (colisão, ponto, vitória); não têm comportamento próprio por quadro. |

## 1. O que se move, muda de estado ou reage a algo?

- `Ball`: move-se todo quadro e muda de direção ao colidir.
- `Paddle` (esquerda e direita): movem-se verticalmente.
- `GameManager`: muda de estado (Menu → Jogando → PontoMarcado → FimDeJogo) e reage a eventos (bola saiu da tela, colisão detectada).

## 2. Propriedades de cada entidade

- `Ball`: posição `(x, y)`, velocidade `(vx, vy)`, raio, velocidade base.
- `Paddle`: posição `(x fixo, y)`, largura, altura, velocidade, limites de tela, controlador usado.
- `GameManager`: estado atual, placar dos dois lados, dimensões da tela, referências à bola e às raquetes.

## 3. Comportamentos a cada quadro

- `Ball::update(dt)`: atualiza a posição (a saída de tela é detectada pelo `GameManager`).
- `Paddle::update(dt, ballY)`: lê o controlador (humano ou IA) e move o Y, com limite nas bordas da tela.
- `GameManager::update(dt)`: atualiza as entidades, resolve colisões, atualiza o placar, decide o reinício da rodada e aciona som/LED.

## 4. Colisões e reações

A reação **não** é igual para todo par:

| Par | Reação |
|---|---|
| Bola × parede (topo/base) | Inverte só `vy` |
| Bola × raquete | Inverte `vx` e ajusta `vy` conforme o ponto de impacto (gera "efeito") |
| Bola × borda lateral (fora do alcance da raquete) | Não é colisão física: é evento de pontuação (reseta a bola e incrementa o placar) |

## 5. Comunicação entre entidades

Sempre por um terceiro: o `GameManager` detecta a colisão entre `Ball` e `Paddle` e chama os métodos de cada uma (`ball.bounceOffPaddle(...)`). Nenhuma guarda ponteiro para a outra.

Exceção consciente: a IA precisa da posição da bola. Isso é resolvido passando `ballY` como parâmetro em `Paddle::update(dt, ballY)`, sem a `Paddle` manter referência à `Ball`.

## 6. O que parece entidade mas não é

- **Placar/HUD:** só dado guardado no `GameManager`.
- **Buzzer e LED RGB:** atuadores que só executam quando o `GameManager` manda.

## 7. Comportamento repetido

A raquete esquerda e a direita são a **mesma classe** `Paddle`. A única diferença é a origem do input, resolvida com a interface `IPaddleController`, com duas implementações: `HumanController` (joystick) e `AiController` (persegue a bola). Isso evita duplicar lógica de movimento e desenho.

As colisões com a parede do topo e da base também têm o mesmo comportamento (inverter `vy`) e usam uma única checagem.

## Reflexão

A pergunta que mais travou foi perceber que "sair pela lateral" não deve ser tratado como colisão igual às outras, e sim como um evento de jogo separado.
