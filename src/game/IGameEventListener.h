#pragma once
#include "GameEvent.h"

// Qualquer coisa que queira reagir a eventos do jogo (som, luz, log de
// depuração, um futuro placar sonoro etc.) implementa esta interface e se
// registra no GameManager via addListener(). O GameManager não conhece
// NENHUMA implementação concreta — só esta interface (padrão Observer).
class IGameEventListener {
public:
    virtual ~IGameEventListener() = default;
    virtual void onGameEvent(GameEvent event) = 0;
};
