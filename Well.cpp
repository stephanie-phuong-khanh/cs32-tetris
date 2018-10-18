#include "Well.h"
#include "Piece.h"
#include "UserInterface.h"
#include <cmath>

using namespace std;

Well::Well(int width, int height)
: matrix(height, vector<char>(width, ' ')), m_width(width), m_height(height)        //Creates 2d vector of ' ' chars, with correct Well size
{   }

void Well::display(Screen& screen, int x, int y) const
{
    screen.refresh();
    screen.gotoXY(x,y);
    for (int vertical=0; vertical<m_height; vertical++)
    {
        screen.printChar('@');                                      //Prints left wall of well
        for (int horizontal=0; horizontal<m_width; horizontal++)    //Prints out well
        {
            screen.printChar(matrix[vertical][horizontal]);
            
        }
        screen.printChar('@');                                       //Prints right wall of well
        screen.printChar('\n');
    }
    for (int x=0; x<m_width+2; x++)
        screen.printChar('@');                                       //Prints bottom wall of well
}

void Well::eraseCurrent()
{
    for (int i=0; i<m_height; i++)
    {
        for (int x=0; x<m_width; x++)
        {
            if (matrix[i][x] == '#')        //Erases the currrent moving Piece from well
                matrix[i][x] = ' ';
        }
    }
}

void Well::eraseEverything()
{
    for (int i=0; i<m_height; i++)
    {
        for (int x=0; x<m_width; x++)           //Completely clears well. Used when starting new level
            matrix[i][x] = ' ';
    }
}

bool Well::okayToPlace(Piece* pointerToPiece, const int& height, const int& width) const  //Checks if piece can be placed at x and y coordinates given (which are called "height" and "width" instead of x and y coordinates)
{
    for (int i=0; i<pointerToPiece->returnPiece().size(); i++)
    {
        int checkVert = height + pointerToPiece->returnPiece()[i].how_much_down;
        int checkHor = width + pointerToPiece->returnPiece()[i].how_much_right;
        if (checkVert>m_height-1 || checkVert<0 || checkHor<0 || checkHor>m_width-1 || !(matrix[checkVert][checkHor] == ' ' || matrix[checkVert][checkHor] == '#'))         //Cannot be placed outside boundaries of Well or where there is anything except empty space or '#'
            return false;
    }
    return true;
}


void Well::piecePlace(Piece* pointerToPiece, const int& height, const int& width)
{
    eraseCurrent();
    for (int i=0; i<pointerToPiece->returnPiece().size(); i++)                      //Makes piece part of well by reading from coordinates of Piece where there are '#' characters
    {
        int replaceVert = height + pointerToPiece->returnPiece()[i].how_much_down;
        int replaceHor = width + pointerToPiece->returnPiece()[i].how_much_right;
        matrix[replaceVert][replaceHor] = '#';
    }
}


void Well::integratePieces()                    //Transforms a piece in play to part of Well as stationary Piece done moving
{
    for (int i=0; i<m_height; i++)
    {
        for (int x=0; x<m_width; x++)
        {
            if (matrix[i][x] == '#')
                matrix[i][x] = '$';
        }
    }
}


int Well::checkForCompleteAndReplace()          //Checks for if a Row is filled with '$' or '*' and should be deleted
{
    int scoreIncrease = 0;
    for (int h=m_height-1; h>=0; h--)           //Will check for completed rows from the bottom up
    {
        bool completeRow = true;
        for (int w=0; w<m_width; w++)
        {
            if (!(matrix[h][w] == '$' || matrix[h][w] == '*'))
            {
                completeRow = false;            //Any row not completely made of * and $ is considered not complete and not to be deleted
                break;
            }
        }
        if (completeRow)                                                //If row is to be deleted:
        {
            scoreIncrease++;
            for (int shiftVert=h; shiftVert>0; shiftVert--)             //Starting from said row, shift every row down
            {
                for (int shiftIndividual=0; shiftIndividual<m_width; shiftIndividual++)
                {
                    matrix[shiftVert][shiftIndividual] = matrix[shiftVert-1][shiftIndividual];
                }
                for (int shiftIndividual=0; shiftIndividual<m_width; shiftIndividual++)     //Top row doesn't copy any information from any row above because there is nothing above. Instead it becomes an empty row of ' '
                {
                    matrix[0][shiftIndividual] = ' ';
                }
            }
            h++;                        //Checks same row again for completeness because we haven't checked that row of characters yet as it has just been shifted down
        }
    }
    return scoreIncrease;                       //Returns how many rows were deleted in total. Will be used to calculate change in score
}


void Well::executeVapor()
{
    for (int h=0; h<m_height; h++)
    {
        for (int w=0; w<m_width; w++)
        {
            if (matrix[h][w] == '#')
            {     //Checks coordinate two above and two below, one above and one below if it is in bounds of the well. Makes that coordinate into ' ' if it can be done
                if (h-1>0)
                    matrix[h-1][w] = ' ';
                if (h-1>0)
                    matrix[h-2][w] = ' ';
                if (h+1<m_height)
                    matrix[h+1][w] = ' ';
                if (h+2<m_height)
                    matrix[h+2][w] = ' ';
                matrix[h][w] = ' ';     //Finally transforms the bomb's own coordinate to ' ' char
            }
        }
    }
}



bool Well::executeFoam(const int &foamStartVert, const int &foamStartHor, int foamCurrentVert, int foamCurrentHor)
{
    int difVert = abs(foamCurrentVert - foamStartVert);
    int difHor = abs(foamCurrentHor - foamStartHor);
    
    //BASE CASE -  we are at a corner of the 5 by 5 box and there is no other empty spaces (' ') to fill with foam
    if ((difVert==2 && difHor==2) && (matrix[foamCurrentVert-1][foamCurrentHor] != ' ') && (matrix[foamCurrentVert+1][foamCurrentHor] != ' ') && (matrix[foamCurrentVert][foamCurrentHor-1] != ' ') && (matrix[foamCurrentVert][foamCurrentHor+1] != ' '))
    {
        matrix[foamCurrentVert][foamCurrentHor] = '*';
        return true;
    }
    
    if  ((difVert<=2 && difHor<=2))     //If we are not yet at said corner, we will keep expanding and exploring new adjacent places to spread the foam
    {
        matrix[foamCurrentVert][foamCurrentHor] = '*';
        bool moveUp(false), moveDown(false), moveRight(false), moveLeft(false);
        
        if (foamCurrentVert>0 && matrix[foamCurrentVert-1][foamCurrentHor] == ' ')   //UP
        {
            if (executeFoam(foamStartVert, foamStartHor, foamCurrentVert-1, foamCurrentHor))        //recursive call
                moveUp = true;
        }
        
        if (foamCurrentVert<m_height-1 && matrix[foamCurrentVert+1][foamCurrentHor] == ' ')   //DOWN
        {
            if (executeFoam(foamStartVert, foamStartHor, foamCurrentVert+1, foamCurrentHor))        //recursive call
                moveDown = true;
        }
        
        if (foamCurrentHor<m_width-1 && matrix[foamCurrentVert][foamCurrentHor+1] == ' ')   //RIGHT
        {
            if (executeFoam(foamStartVert, foamStartHor, foamCurrentVert, foamCurrentHor+1))        //recursive call
                moveRight = true;
        }
        
        if (foamCurrentVert>0 && matrix[foamCurrentVert][foamCurrentHor-1] == ' ')   //LEFT
        {
            if (executeFoam(foamStartVert, foamStartHor, foamCurrentVert, foamCurrentHor-1))        //recursive call
                moveLeft = true;
        }
        return (moveUp || moveDown || moveRight || moveLeft);      //Returns false only when there is a dead end and no direction can be taken to furthur fill with foam
    }
    else
        return false;
}

