#ifndef WELL_INCLUDED
#define WELL_INCLUDED


#include <vector>

class Screen;
class Piece;

class Well
{
public:
    Well(int width, int height);
    void display(Screen& screen, int x, int y) const; 
    bool okayToPlace(Piece* pointerToPiece, const int& height, const int& width) const;
    void piecePlace(Piece* pointerToPiece, const int& height, const int& width);
    void eraseCurrent();
    void eraseEverything();
    void integratePieces();
    int checkForCompleteAndReplace();
    void executeVapor();
    bool executeFoam(const int& foamStartVert, const int& foamStartHor, int foamCurrentVert, int foamCurrentHor);
private:
    std::vector<std::vector<char> >matrix;
    int m_width;
    int m_height;
};

#endif // WELL_INCLUDED

