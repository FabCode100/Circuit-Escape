#ifndef GAME_STATE_H
#define GAME_STATE_H

enum class GameState {
    EXPLORATION,   // Explorando o mapa/labirinto
    PUZZLE,        // Montando circuito no puzzle
    DIALOGUE,      // Diálogo com Professor Leo
    VICTORY        // Fim da Fase 1 - Tela de Sucesso
};

#endif // GAME_STATE_H
