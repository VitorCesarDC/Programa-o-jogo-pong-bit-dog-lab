# ENTIDADES.md

**Nome(s):** _(preencha com seu nome / do grupo)_

## Entidades do jogo

- `Ball` (bola)
- `Paddle` (raquete — mesma classe para jogador humano e IA)
- `GameManager` (não é bem uma "entidade" de jogo; é o mediador/árbitro — ver pergunta 6)

## 1. O que se move, muda de estado ou reage a algo?

- `Ball`: move-se todo frame e muda de direção ao colidir.
- `Paddle` esquerda (joystick) e `Paddle` direita (IA): movem-se verticalmente.
- `GameManager`: muda de estado (Menu → Jogando → PontoMarcado → FimDeJogo) e reage a eventos (bola saiu da tela, colisão detectada).

## 2. O que cada entidade sabe sobre si (propriedades)?

- `Ball`: posição `(x, y)`, velocidade `(vx, vy)`, raio, velocidade base.
- `Paddle`: posição `(x fixo, y)`, largura, altura, velocidade, limites de tela, qual controlador usa.
- `GameManager`: estado atual, placar dos dois lados, dimensões da tela, referências à bola e às raquetes.

## 3. O que cada uma faz a cada quadro (comportamentos)?

- `Ball::update(dt)`: atualiza posição; detecção de saída de tela é feita por fora (GameManager).
- `Paddle::update(dt, ballY)`: lê o controlador (humano ou IA) e move o Y, com clamp nos limites da tela.
- `GameManager::update(dt)`: chama o update de todas as entidades, resolve colisões, atualiza placar, decide reinício de rodada e aciona som/LED.

## 4. O que colide com o quê? A reação é igual para todo par?

**Não.** Três pares diferentes, três reações diferentes:

| Par | Reação |
|---|---|
| Bola x parede (topo/base) | Inverte só `vy` |
| Bola x raquete | Inverte `vx` e ajusta `vy` conforme ponto de impacto (dá "efeito") |
| Bola x borda lateral (fora do alcance da raquete) | **Não é colisão física** — é evento de pontuação: reseta bola, incrementa placar |

Essa foi a pergunta que mais travou: perceber que "sair pela lateral" não deveria ser tratado como uma colisão igual às outras, e sim como um evento de jogo separado.

## 5. Uma entidade aciona ou lê a outra direto, ou por um terceiro?

Por um terceiro: o `GameManager` detecta a colisão entre `Ball` e `Paddle` e chama os métodos de cada uma (`ball.bounceOffPaddle(...)`). Nenhuma das duas guarda ponteiro para a outra.

Exceção consciente: a IA da raquete precisa da posição da bola para decidir o movimento. Isso é resolvido passando `ballY` como parâmetro em `Paddle::update(dt, ballY)`, em vez da `Paddle` manter uma referência direta à `Ball`.

## 6. Algo parece entidade, mas não se atualiza sozinho?

- **Placar/HUD**: é só um dado guardado no `GameManager` e desenhado pelo renderer — não tem posição nem `update()` próprio.
- **Buzzer e LED RGB**: são atuadores acionados por eventos (colisão, ponto), não têm comportamento próprio a cada frame; só executam quando o `GameManager` manda.

## 7. Duas coisas repetem o mesmo comportamento?

Sim — a raquete esquerda e a direita são a **mesma classe** `Paddle`. A única diferença é de onde vem o input, resolvida com uma interface `IPaddleController` (estratégia), com duas implementações: `HumanController` (lê o joystick) e `AiController` (persegue a bola). Isso evitou duplicar a lógica de movimento e desenho em duas classes separadas.

Também a colisão bola-parede-topo e bola-parede-base têm o mesmo comportamento (inverter `vy`), tratadas por uma única checagem, sem duplicação.
