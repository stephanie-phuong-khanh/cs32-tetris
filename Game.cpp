#include "Game.h"
#include "Well.h"
#include "UserInterface.h"
#include "Piece.h"
#include <iostream>
#include <string>
#include <queue>
#include <algorithm>
using namespace std;

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;

const int WELL_X = 0;
const int WELL_Y = 0;

const int PROMPT_Y = 20;
const int PROMPT_X = 0;

const int SCORE_X = 16;
const int SCORE_Y = 8; 

const int ROWS_LEFT_X = 16;
const int ROWS_LEFT_Y = 9;

const int LEVEL_X = 16;
const int LEVEL_Y = 10;

const int NEXT_PIECE_TITLE_X = 16;
const int NEXT_PIECE_TITLE_Y = 3;

const int NEXT_PIECE_X = 16;
const int NEXT_PIECE_Y = 4;

Game::Game(int width, int height)
: m_well(width, height), m_screen(SCREEN_WIDTH, SCREEN_HEIGHT), m_level(1), m_score(0), m_rowsLeft(5*m_level), m_pieceQueue()
{
    enqueuePiece();
    enqueuePiece();
}

void Game::play()
{
    m_well.display(m_screen, WELL_X, WELL_Y);
    displayStatus(false);                   //  score, rows left, level
    displayPrompt("Press the Enter key to begin playing Chetyris!");
    waitForEnter();                         // [in UserInterface.h]
   
    for(;;)
    {
        displayStatus(true);                //Displays score, rows left, and level accordingly
        m_well.display(m_screen, WELL_X, WELL_Y);
        if (!playOneLevel())
            break;
        
        //Player completes a level
        displayPrompt("Good job!  Press the Enter key to start next level!");
        waitForEnter();
        
        m_well.eraseEverything();       //Well is cleared
        m_level++;                      //Level is increased
        m_rowsLeft = 5*m_level;         //Game gets harder with increased level
        
        changePiece();                  //Empties and reloads queue of pieces
        changePiece();
    }
    while(!m_pieceQueue.empty())       //When game ends, clear dynamically allocated Pieces in queue
    {
        delete m_pieceQueue.front();
        m_pieceQueue.pop();
    }
    displayPrompt("Game Over!  Press the Enter key to exit!");  //Runs when player loses
    waitForEnter();
}

void Game::displayPrompt(std::string s)
{
    m_screen.gotoXY(PROMPT_X, PROMPT_Y);
    m_screen.printStringClearLine(s);   // Overwrites previous text
}

void Game::displayStatus(bool displayNext)
{
    if (displayNext)
    {
        // PRINTING THE NEXT PIECE //
        m_screen.gotoXY(NEXT_PIECE_TITLE_X, NEXT_PIECE_TITLE_Y);
        m_screen.printStringClearLine("Next piece:");
        
        string row0("    "), row1("    "), row2("    "), row3("    ");   //Empty strings to be filled
        for (int i=0; i<m_pieceQueue.back()->returnPiece().size(); i++)  //Gets info from back/second piece in queue, the upcoming piece
        {    //Fills empty strins above with according #'s, essentially rebuilding the Piece in the form of 4 strings (rows of the Piece) to be printed
            if (m_pieceQueue.back()->returnPiece()[i].how_much_down == 0)
                row0[m_pieceQueue.back()->returnPiece()[i].how_much_right] = '#';
            else if (m_pieceQueue.back()->returnPiece()[i].how_much_down == 1)
                row1[m_pieceQueue.back()->returnPiece()[i].how_much_right] = '#';
            else if (m_pieceQueue.back()->returnPiece()[i].how_much_down == 2)
                row2[m_pieceQueue.back()->returnPiece()[i].how_much_right] = '#';
            else if (m_pieceQueue.back()->returnPiece()[i].how_much_down == 3)
                row3[m_pieceQueue.back()->returnPiece()[i].how_much_right] = '#';
        }
        m_screen.gotoXY(NEXT_PIECE_X, NEXT_PIECE_Y);    //Prints each string individually
        m_screen.printStringClearLine(row0);
        m_screen.gotoXY(NEXT_PIECE_X, NEXT_PIECE_Y+1);
        m_screen.printStringClearLine(row1);
        m_screen.gotoXY(NEXT_PIECE_X, NEXT_PIECE_Y+2);
        m_screen.printStringClearLine(row2);
        m_screen.gotoXY(NEXT_PIECE_X, NEXT_PIECE_Y+3);
        m_screen.printStringClearLine(row3);
    }
        
    m_screen.gotoXY(SCORE_X, SCORE_Y);                      //Prints score
    m_screen.printStringClearLine("Score:    ");
    m_screen.gotoXY(SCORE_X+10, SCORE_Y);
    int scoreLength = (int)to_string(m_score).length();     //Right-aligns values printed in the field of 7 characters
    int scoreSpaces = 7-scoreLength;
    string printedScore;
    for (int i=0; i<scoreSpaces; i++)
        printedScore += ' ';
    printedScore += to_string(m_score);
    m_screen.printStringClearLine(printedScore);
    
    m_screen.gotoXY(ROWS_LEFT_X, ROWS_LEFT_Y);              //Prints rows left
    m_screen.printStringClearLine("Rows left: ");
    m_screen.gotoXY(ROWS_LEFT_X+10, ROWS_LEFT_Y);
    int rowsLeftLength = (int)to_string(m_rowsLeft).length();   //Right-aligns values printed in the field of 7 characters
    int rowsLeftSpaces = 7-rowsLeftLength;
    string printedRowsLeft;
    for (int i=0; i<rowsLeftSpaces; i++)
        printedRowsLeft += ' ';
    printedRowsLeft += to_string(m_rowsLeft);
    m_screen.printStringClearLine(printedRowsLeft);
    
    m_screen.gotoXY(LEVEL_X, LEVEL_Y);                      //Prints level
    m_screen.printStringClearLine("Level:     ");
    m_screen.gotoXY(LEVEL_X+10, LEVEL_Y);
    int levelLength = (int)to_string(m_level).length(); //Right-aligns values printed in the field of 7 characters
    int levelSpaces = 7-levelLength;
    string printedLevel;
    for (int i=0; i<levelSpaces; i++)
        printedLevel += ' ';
    printedLevel += to_string(m_level);
    m_screen.printStringClearLine(printedLevel);
}



bool Game::playOneLevel()
{
    const int startHeight(0), startWidth(3);
    
    Timer t;
    
    //This loop runs for the appearance, fall, and integration of each piece//
    while (m_rowsLeft > 0)
    {
        //Set pointers accordingly to Piece at front of queue of pieces//
        Piece* pointerToCurrent = m_pieceQueue.front();
        displayStatus(true);
        
        //First position of piece//
        if (!m_well.okayToPlace(pointerToCurrent, startHeight, startWidth))
        {   //New piece directly overlaps existing pieces
            m_well.piecePlace(pointerToCurrent, startHeight, startWidth);
            m_well.display(m_screen, WELL_X, WELL_Y);
            displayStatus(true);
            return false;                   //Shows where it would have fit, game ends
        }
        m_well.piecePlace(pointerToCurrent, startHeight, startWidth);   //It is okay to start piece
        m_well.display(m_screen, WELL_X, WELL_Y);
        int currentHeight(startHeight), currentWidth(startWidth);
        displayStatus(true);
        
        bool reachedBottom = false;
        while (!reachedBottom)              //Runs for duration of Piece's descent
        {
            t.start();
            //Player has the duration of this loop to shift, rotate, space//
            while (t.elapsed() < max(1000-(100*(m_level-1)), 100))
            {
                char command;
                if (getCharIfAny(command))
                {
                    if (command == 'Q' || command == 'q')         //Quit if player has hit q or Q
                        return false;
                    else if (command == ARROW_LEFT || command == '4' || command == 'a')    //LEFT when player has hit left key or equivalent command
                    {
                        if (pointerToCurrent->isACrazy())   //Will move right for Crazy Piece
                        {
                            if (m_well.okayToPlace(pointerToCurrent, currentHeight, currentWidth+1))
                            {
                                currentWidth++;  //Updates current horizontal position, makes well place piece in new position
                                m_well.piecePlace(pointerToCurrent, currentHeight, currentWidth);
                                m_well.display(m_screen, WELL_X, WELL_Y);   //Will display updated Well after each change to m_well
                                displayStatus(true);
                            }
                        }
                        else                                //Moves normally for non-Crazy Piece
                        {
                            if (m_well.okayToPlace(pointerToCurrent, currentHeight, currentWidth-1))
                            {
                                currentWidth--;
                                m_well.piecePlace(pointerToCurrent, currentHeight, currentWidth);
                                m_well.display(m_screen, WELL_X, WELL_Y);
                                displayStatus(true);
                            }
                        }
                    }
                    else if (command == ARROW_RIGHT || command == '6' || command == 'd')    //RIGHT when layer has hit right key or equivalent command
                    {
                        if (pointerToCurrent->isACrazy())   //Move left for Crazy Piece
                        {
                            if (m_well.okayToPlace(pointerToCurrent, currentHeight, currentWidth-1))
                            {
                                currentWidth--;
                                m_well.piecePlace(pointerToCurrent, currentHeight, currentWidth);
                                m_well.display(m_screen, WELL_X, WELL_Y);
                                displayStatus(true);
                            }
                        }
                        else                               //Moves normally for non-Crazy Piece
                        {
                            if (m_well.okayToPlace(pointerToCurrent, currentHeight, currentWidth+1))
                            {
                                currentWidth++;
                                m_well.piecePlace(pointerToCurrent, currentHeight, currentWidth);
                                m_well.display(m_screen, WELL_X, WELL_Y);
                                displayStatus(true);
                            }
                        }
                    }
                    else if (command == ARROW_UP || command == '8' || command == 'w')  //ROTATE when player has hit up key or equivalent command
                    {
                        pointerToCurrent->rotate(); //Rotates piece, checks if the rotation can be done
                        if (m_well.okayToPlace(pointerToCurrent, currentHeight, currentWidth))
                        {   //Makes rotated piece part of Well if rotation is okay
                            m_well.piecePlace(pointerToCurrent, currentHeight, currentWidth);
                            m_well.display(m_screen, WELL_X, WELL_Y);
                            displayStatus(true);
                        }
                        else   //If rotation is unsuccessful, rotate the piece back to original position by doing three successive rotations
                        {
                            pointerToCurrent->rotate();
                            pointerToCurrent->rotate();
                            pointerToCurrent->rotate();
                        }
                    }
                    else if (command == ARROW_DOWN || command == '2' || command == 's')  //DOWN when player has hit down key or equivalent command
                    {
                        if (m_well.okayToPlace(pointerToCurrent, currentHeight+1, currentWidth))
                        {
                            currentHeight++;
                            m_well.piecePlace(pointerToCurrent, currentHeight, currentWidth);
                            m_well.display(m_screen, WELL_X, WELL_Y);
                            displayStatus(true);
                            t.start();         //Restarts timer when piece is shifted down
                        }
                    }
                    else if (command == ' ')    //SPACE when player has hit space bar
                    {
                        bool canBeMovedDown = true;
                        while (canBeMovedDown)
                        {   //Continually moves pieces down until it must stop
                            if (m_well.okayToPlace(pointerToCurrent, currentHeight+1, currentWidth))
                            {
                                currentHeight++;
                                m_well.piecePlace(pointerToCurrent, currentHeight, currentWidth);
                                m_well.display(m_screen, WELL_X, WELL_Y);
                                displayStatus(true);
                            }
                            else
                                canBeMovedDown = false; //Piece stops falling
                        }
                        break;  //Breaks out of loop of time frame for pressing keys
                    }
                }       //End of while loop of time frame for pressing shift/rotate/space keys
            }           //End of while loop started in line 176, a piece's lifetime
        
            //Piece drops naturally at the end of time frame//
            if (m_well.okayToPlace(pointerToCurrent, currentHeight+1, currentWidth))
            {
                currentHeight++;
                m_well.piecePlace(pointerToCurrent, currentHeight, currentWidth);
                m_well.display(m_screen, WELL_X, WELL_Y);
                displayStatus(true);
            }
            else
            {
                if (pointerToCurrent->isAVaporBomb())   //Executes actions of Vapor Bomb if needed
                    m_well.executeVapor();
                else if (pointerToCurrent->isAFoam())   //Executes actions of Foam Bomb if needed
                    m_well.executeFoam(currentHeight+1, currentWidth+1, currentHeight+1, currentWidth+1);
                m_well.integratePieces();               //Finally integrates fallen piece into well (# to $)
                m_well.display(m_screen, WELL_X, WELL_Y);
                displayStatus(true);
                reachedBottom = true;
            }
        }
        
        //Checks for any completed rows//
        int rowsDeleted = m_well.checkForCompleteAndReplace();
        m_rowsLeft -= rowsDeleted;
        if (m_rowsLeft < 0)    //In case m_rowsLeft becomes negative from many rows completed at once
            m_rowsLeft = 0;    //Makes sure value never becomes negative
        switch (rowsDeleted)    //Updates score accordingly
        {
            case (0):
                break;
            case (1):
                m_score += 100;
                break;
            case (2):
                m_score += 200;
                break;
            case (3):
                m_score += 400;
                break;
            case (4):
                m_score += 800;
                break;
            case (5):
                m_score += 1600;
                break;
        }
        m_well.display(m_screen, WELL_X, WELL_Y);
        changePiece();
        displayStatus(true);
    }
    return true;    //return true to keep going, play another level
}


////////////////////////////////////////////////
/////////////   HELPER FUNCTIONS   /////////////
////////////////////////////////////////////////
void Game::changePiece()
{
    delete m_pieceQueue.front();        //When piece is changed out, its space on the heap is freed
    m_pieceQueue.pop();                 //Pops the top pointer to piece
    enqueuePiece();
}

void Game::enqueuePiece()
{
    Piece* ptrToPiece;                  //Creates new pointer to a Piece
    bool foundIt = false;               //We have not yet found a valid choice for a new piece
    while (!foundIt)                    //Runs as long as this bool is false
    {
        foundIt = true;
        switch(chooseRandomPieceType())
        {
            case PIECE_T:
                ptrToPiece = new Piece_T;
                break;
            case PIECE_L:
                ptrToPiece = new Piece_L;
                break;
            case PIECE_J:
                ptrToPiece = new Piece_J;
                break;
            case PIECE_O:
                ptrToPiece = new Piece_O;
                break;
            case PIECE_S:
                ptrToPiece = new Piece_S;
                break;
            case PIECE_Z:
                ptrToPiece = new Piece_Z;
                break;
            case PIECE_I:
                ptrToPiece = new Piece_I;
                break;
            case PIECE_VAPOR:
                ptrToPiece = new Piece_Vapor;
                break;
            case PIECE_FOAM:
                ptrToPiece = new Piece_Foam;
                break;
            case PIECE_CRAZY:
                ptrToPiece = new Piece_Crazy;
                break;
            default:    //Invalid piece --> foundIt becomes false
                foundIt = false;
                break;
        }
        if (foundIt)    //Runs when an invalid Piece is returned from random function, and we run the loop again to generate another random piece. If the "default" case in the switch statement runs, we do NOT break out of this while loop and we run it again
            break;
    }
    m_pieceQueue.push(ptrToPiece);
}



