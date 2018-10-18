#ifndef PIECE_INCLUDED
#define PIECE_INCLUDED

#include <iostream>
#include <vector>
#include <string>
using namespace std;

typedef std::vector<std::vector<char> > vector2d;
enum PieceType {
    PIECE_I, PIECE_L, PIECE_J, PIECE_T, PIECE_O, PIECE_S, PIECE_Z,
    PIECE_VAPOR, PIECE_FOAM, PIECE_CRAZY, NUM_PIECE_TYPES
};


struct coordinate       //Coordinate system to keep track of where in the 4 by 4 Piece box there is a #, where the Piece actually exists
{
    int how_much_down;  //aka Y coordinate
    int how_much_right; //aka X coordinate
};


class CircularList
{
public:
    CircularList();
    void rotate();
    vector<coordinate>* accessor();
    void specifyValues(string pos0, string pos1, string pos2, string pos3);
private:
    int current;                            //Keeps track of which orientation we are on
    vector<coordinate> orientation_0;
    vector<coordinate> orientation_1;
    vector<coordinate> orientation_2;
    vector<coordinate> orientation_3;
};

class Piece
{
public:
    Piece(string pos0, string pos1, string pos2, string pos3);
    void rotate();
    vector<coordinate> returnPiece();
    virtual bool isACrazy() const = 0;
    virtual bool isAVaporBomb() const = 0;
    virtual bool isAFoam() const = 0;
    virtual ~Piece();
private:
    CircularList orientations;
};


class Normal : public Piece
{
public:
    Normal(string pos0, string pos1, string pos2, string pos3);
    virtual bool isACrazy() const;
    virtual bool isAVaporBomb() const;
    virtual bool isAFoam() const;
    virtual ~Normal();
private:
};

class Special : public Piece
{
public:
    Special(string pos0, string pos1, string pos2, string pos3);
    virtual bool isACrazy() const = 0;
    virtual bool isAVaporBomb() const = 0;
    virtual bool isAFoam() const = 0;
    virtual ~Special();
private:
};


class Piece_I : public Normal
{   public: Piece_I();
            ~Piece_I();   };
class Piece_T : public Normal
{   public: Piece_T();
            ~Piece_T();   };
class Piece_L : public Normal
{   public: Piece_L();
            ~Piece_L();   };
class Piece_J : public Normal
{   public: Piece_J();
            ~Piece_J();   };
class Piece_O : public Normal
{   public: Piece_O();
            ~Piece_O();   };
class Piece_S : public Normal
{   public: Piece_S();
            ~Piece_S();   };
class Piece_Z : public Normal
{   public: Piece_Z();
            ~Piece_Z();   };


class Piece_Vapor : public Special
{
public:
    Piece_Vapor();
    virtual bool isACrazy() const;
    virtual bool isAVaporBomb() const;
    virtual bool isAFoam() const;
    ~Piece_Vapor();
};

class Piece_Foam : public Special
{
public:
    Piece_Foam();
    virtual bool isACrazy() const;
    virtual bool isAVaporBomb() const;
    virtual bool isAFoam() const;
    ~Piece_Foam();
};

class Piece_Crazy : public Special
{
public:
    Piece_Crazy();
    virtual bool isACrazy() const;
    virtual bool isAVaporBomb() const;
    virtual bool isAFoam() const;
    ~Piece_Crazy();
};

#endif // PIECE_INCLUDED
