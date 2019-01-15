#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
    

    
private: //!!!
    struct shipinfo{
        int m_length;
        char m_symbol;
        string nameofShip;
    };
    int m_rows;
    int m_cols;
    int numofShip;
    vector<shipinfo> m_shiplist;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
{
        m_rows = nRows;
        m_cols = nCols;
    numofShip = 0;
}

int GameImpl::rows() const    //return the row of the game
{
    return m_rows;
}

int GameImpl::cols() const    //return the col of the game
{
    return m_cols;
}

bool GameImpl::isValid(Point p) const     //return if this point is valid
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const         //return a random Point
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)       //add a ship
{
    if(length<0||(length>m_cols&&length>m_rows)){    //check if length is valid
        return false;
    }
    if(symbol=='X' || symbol=='.' || symbol=='o'){      //check if symbol is valid
        return false;
    }
    for(int i=0;i<numofShip;i++){
        if(m_shiplist[i].m_symbol==symbol){
            return false;
        }
    }
    shipinfo x;
    x.m_length = length;
    x.m_symbol = symbol;
    x.nameofShip = name;
    m_shiplist.push_back(x);
    numofShip++;
    return true;
}

int GameImpl::nShips() const      //return the number of ships
{
    return numofShip;
}

int GameImpl::shipLength(int shipId) const     //return the length of ship
{
    return m_shiplist[shipId].m_length;
}

char GameImpl::shipSymbol(int shipId) const     //return the ship symbol
{
    return m_shiplist[shipId].m_symbol;
}

string GameImpl::shipName(int shipId) const  //return the ship name
{
    return m_shiplist[shipId].nameofShip;
}



Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    bool shotHit1=false;
    bool shipDestroyed1 = false;
    int shipId1 =0;
    bool shotHit2=false;
    bool shipDestroyed2 = false;
    int shipId2 =0;
    if(!p1->placeShips(b1)){       //return nullptr if p1 cannot placeships
        return nullptr;
    }
    if(!p2->placeShips(b2)){     //return nullptr if p2 cannot placeships
        return nullptr;
    }
    while(!b1.allShipsDestroyed()&&!b2.allShipsDestroyed()){   //while not all ships are destroyed
        
        cout<<p1->name()<<"'s turn.  Board for "<<p2->name()<<":"<<endl;
        if(p1->isHuman()){
            b2.display(true);
        }
        else
            b2.display(false);
        Point attackp1 = p1->recommendAttack();
        if(b2.attack(attackp1, shotHit1, shipDestroyed1, shipId1)){
            p1->recordAttackResult(attackp1, isValid(attackp1),shotHit1, shipDestroyed1, shipId1);
            if(shipDestroyed1&&shotHit1){
                cout<<p1 ->name()<<" attacked ("<<attackp1.r<<","<<attackp1.c<<") and destroyed the "<<p1->game().shipName(shipId1)<<", resulting in:"<<endl;
            }
            else if(shotHit1&&!shipDestroyed1){
                cout<<p1->name()<<" attacked ("<<attackp1.r<<","<<attackp1.c<<") and hit something, resulting in:"<<endl;
            }
            else{
                cout<<p1->name()<<" attacked ("<<attackp1.r<<","<<attackp1.c<<") and missed, resulting in:"<<endl;
            }
            if(p1->isHuman()){
                b2.display(true);
            }
            else
                b2.display(false);
            
        }
        else{
            cout<<p1 ->name()<<" the Human wasted a shot at ("<<attackp1.r<<","<<attackp1.c<<")"<<endl;
        }
        
        
        
        
        if(b2.allShipsDestroyed()){
            if(p2->isHuman()){
                b1.display(false);
            }
            cout<<p1->name()<<" wins!"<<endl;
            b1.clear();
            b2.clear();
            return p1;
        }
        
        if(shouldPause){
            waitForEnter();
        }
        
        
        cout<<p2->name()<<"'s turn.  Board for "<<p1->name()<<":"<<endl;
        if(p2->isHuman()){
            b1.display(true);
        }
        else
            b1.display(false);
        Point attackp2 = p2->recommendAttack();
        if(b1.attack(attackp2, shotHit2, shipDestroyed2, shipId2)){
            p2->recordAttackResult(attackp2, isValid(attackp2),shotHit2, shipDestroyed2, shipId2);
            if(shipDestroyed2&&shotHit2){
                cout<<p2 ->name()<<" attacked ("<<attackp2.r<<","<<attackp2.c<<") and destroyed the "<<p2->game().shipName(shipId2)<<", resulting in:"<<endl;
            }
            else if(shotHit2&&!shipDestroyed2){
                cout<<p2->name()<<" attacked ("<<attackp2.r<<","<<attackp2.c<<") and hit something, resulting in:"<<endl;
            }
            else{
                cout<<p2->name()<<" attacked ("<<attackp2.r<<","<<attackp2.c<<") and missed, resulting in:"<<endl;
            }
            if(p2->isHuman()){
                b1.display(true);
            }
            else
                b1.display(false);
        }
        else{
            cout<<p2 ->name()<<" the Human wasted a shot at ("<<attackp2.r<<","<<attackp2.c<<")"<<endl;
        }
        
        
        if(b1.allShipsDestroyed()){
            if(p1->isHuman()){
                b2.display(false);
            }
            cout<<p2->name()<<" wins!"<<endl;
            b1.clear();
            b2.clear();
            return p2;
        }
        
        if(shouldPause){
            waitForEnter();
        }
        
    }
    
    return nullptr;// This compiles but may not be correct
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
        << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
        << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
        << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
            << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

