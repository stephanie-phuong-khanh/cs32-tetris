#ifndef GAME_INCLUDED
#define GAME_INCLUDED

#include "Well.h" 
#include "UserInterface.h"
#include "Piece.h"
#include <string>
#include <queue>

PieceType chooseRandomPieceType();

class Game
{
public:
    Game(int width, int height);
    void play();
    bool playOneLevel();
    void displayPrompt(std::string s);
    void displayStatus(bool displayNext);
private:
    Well    m_well;
    Screen  m_screen;
    int     m_level;
    int     m_score;
    int     m_rowsLeft;
    std::queue<Piece*> m_pieceQueue;
    
    ///// Helper functions /////
    void enqueuePiece();
    void changePiece();
};

#endif // GAME_INCLUDED
