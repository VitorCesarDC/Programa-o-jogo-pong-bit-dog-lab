#pragma once

// Renderer é uma interface mínima para desenho; qualquer driver de display
// concreto (ex.: DisplayDriver, que embrulha a lib ssd1306 do curso) deve
// implementá-la. Isso desacopla as entidades do hardware de display.
class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void drawRect(int x, int y, int w, int h, bool on = true) = 0;
    virtual void drawFilledCircle(int cx, int cy, int r, bool on = true) = 0;
};

// Toda entidade "viva" do jogo (se move e/ou reage a algo) implementa isto.
// Coisas que só são desenhadas (linha central, placar) NÃO precisam ser
// Entity — ver pergunta 6 do checkpoint.
class Entity {
public:
    virtual ~Entity() = default;

    // Atualiza estado interno da entidade. dt em segundos.
    virtual void update(float dt) = 0;

    // Desenha a entidade usando o renderer abstrato.
    virtual void draw(IRenderer& renderer) const = 0;
};
