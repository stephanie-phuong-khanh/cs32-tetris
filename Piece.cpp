#include "Piece.h"
using namespace std;

//CIRCULAR LIST//
CircularList::CircularList()
: current(0)
{   }

void CircularList::rotate()     //Will simply increment "current" member variable accordingly to return correct Piece if asked to be returned
{
    switch (current)
    {
        case 0:
        case 1:
        case 2:
            current++;
            break;
        case 3:
            current = 0;                            //Makes current = 0 if current is 3 and incremented
            break;
    }
}

vector<coordinate>* CircularList::accessor()        //Returns pointer to Orientation of piece that is currently manifested
{
    switch (current)
    {
        case 0:
            return &orientation_0;
            break;
        case 1:
            return &orientation_1;
            break;
        case 2:
            return &orientation_2;
            break;
        default:    //current = 3
            return &orientation_3;
            break;
    }
}

void CircularList::specifyValues(string pos0, string pos1, string pos2, string pos3)
{
    for (int i=0; i<(int)pos0.size();i=i+2)         //For each orientation, we change the strings of numbers pos0, pos1, pos2, and pos3 given by the specified Piece constructors into vectors of coordinates of where there are # in the 4 by 4
    {
        coordinate temp;                            //Create temporary coordinate
        temp.how_much_down = pos0[i]-48;            //Fills up Y-coordinate part of coordinate
        temp.how_much_right = pos0[i+1]-48;         //Fills up X-coordinate part of coordinate
        orientation_0.push_back(temp);              //Adds coordinate to vector
    }
    for (int i=0; i<(int)pos1.size();i=i+2)
    {
        coordinate temp;
        temp.how_much_down = pos1[i]-48;
        temp.how_much_right = pos1[i+1]-48;
        orientation_1.push_back(temp);
    }
    for (int i=0; i<(int)pos2.size();i=i+2)
    {
        coordinate temp;
        temp.how_much_down = pos2[i]-48;
        temp.how_much_right = pos2[i+1]-48;
        orientation_2.push_back(temp);
    }
    for (int i=0; i<(int)pos3.size();i=i+2)
    {
        coordinate temp;
        temp.how_much_down = pos3[i]-48;
        temp.how_much_right = pos3[i+1]-48;
        orientation_3.push_back(temp);
    }
}


////// PIECE /////
Piece::Piece(string pos0, string pos1, string pos2, string pos3)
{   orientations.specifyValues(pos0, pos1, pos2, pos3); }

void Piece::rotate()
{   orientations.rotate();  }

vector<coordinate> Piece::returnPiece()      //Returns copy of vector holding the current coordinates of Piece when asked for
{   return *(orientations.accessor());  }

Piece::~Piece() {}




//// NORMAL ////
Normal::Normal(string pos0, string pos1, string pos2, string pos3)
: Piece (pos0, pos1, pos2, pos3) {}
bool Normal::isACrazy() const                   //None of the Normal pieces will return true for any of the special pieces functions
{   return false;   }
bool Normal::isAVaporBomb() const
{   return false;   }
bool Normal::isAFoam() const
{   return false;   }
Normal::~Normal() {}



//// SPECIAL ////
Special::Special(string pos0, string pos1, string pos2, string pos3)
: Piece (pos0, pos1, pos2, pos3) {}
Special::~Special() {}




////// PIECE T ////
Piece_T::Piece_T()
: Normal("10011112","01112112","10111221","10011121") {}            //Each piece provides 4 strings corresponding to each position, with x-coordinates and y-coordinates stored contiguously. For example, the first string translates into a 4 by 4 piece with # in row 1 column 0, row 0 column 1, row 1 column 1, and row 1 column 2
Piece_T::~Piece_T() {}

////// PIECE L ////
Piece_L::Piece_L()
: Normal("10201112","01021222","20212212","11213132") {}
Piece_L::~Piece_L() {}

////// PIECE J ////
Piece_J::Piece_J()
: Normal("11121323","12223132","11212223","01112102") {}
Piece_J::~Piece_J() {}

////// PIECE O ////
Piece_O::Piece_O()
: Normal("00100111","00100111","00100111","00100111") {}
Piece_O::~Piece_O() {}

////// PIECE S ////
Piece_S::Piece_S()
: Normal("20211112","01111222","20211112","01111222") {}
Piece_S::~Piece_S() {}

////// PIECE Z ////
Piece_Z::Piece_Z()
: Normal("10112122","11211202","10112122","11211202") {}
Piece_Z::~Piece_Z() {}

//// PIECE I ////
Piece_I::Piece_I()
: Normal("10111213","01112131","10111213","01112131") {}
Piece_I::~Piece_I() {}

//// PIECE VAPOR ////
Piece_Vapor::Piece_Vapor()
: Special("0102","0102","0102","0102") {}
bool Piece_Vapor::isACrazy() const
{ return false; }
bool Piece_Vapor::isAVaporBomb() const              //Returns true to only Vapor Bomb function because it is one
{ return true; }
bool Piece_Vapor::isAFoam() const
{ return false; }
Piece_Vapor::~Piece_Vapor() {}

//// PIECE FOAM ////
Piece_Foam::Piece_Foam()
: Special("11","11","11","11") {}
bool Piece_Foam::isACrazy() const
{ return false; }
bool Piece_Foam::isAFoam() const              //Returns true to only Foam Bomb function because it is one
{ return true; }
bool Piece_Foam::isAVaporBomb() const
{ return false; }
Piece_Foam::~Piece_Foam() {}

//// PIECE CRAZY ////
Piece_Crazy::Piece_Crazy()
: Special("00111203","03122233","30212233","00112130") {}
bool Piece_Crazy::isACrazy() const                                //Returns true to only Crazy Piece function because it is one
{ return true; }
bool Piece_Crazy::isAFoam() const
{ return false; }
bool Piece_Crazy::isAVaporBomb() const
{ return false; }
Piece_Crazy::~Piece_Crazy() {}








