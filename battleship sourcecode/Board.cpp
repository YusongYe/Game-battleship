#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

class BoardImpl
{
public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;
    
private:
    // TODO:  Decide what private members you need.  Here's one that's likely
    //        to be useful:
    
    char m_board[MAXROWS][MAXCOLS];
    const Game& m_game;
    vector<int> shiplist;
};

BoardImpl::BoardImpl(const Game& g)
: m_game(g)
{
    for(int i=0;i<g.rows();i++)             //set an empty board
    {
        for(int j=0; j<g.cols();j++){
            m_board[i][j]='.';
        }
    }
}

void BoardImpl::clear()
{
    for(int i=0;i<m_game.rows();i++){
        for(int j=0; j<m_game.cols();j++){
            m_board[i][j]='.';
        }
    }
    
}

void BoardImpl::block()
{
    // Block cells with 50% probability
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
            if (randInt(2) == 0)
            {
                m_board[r][c]='#';
            }
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            if(m_board[r][c]=='#'){
                m_board[r][c]='.';
            }
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    
    int count=0;     //count number of points that is valid for adding ship
    if(shipId<0||shipId>=m_game.nShips()){
        return false;
    }
    if(dir==VERTICAL){
        int temp =m_game.shipLength(shipId);
        if(topOrLeft.r+m_game.shipLength(shipId)>m_game.rows()-1){     //check if the ship can fit the board
            return false;
        }
        for(int i=topOrLeft.r;i<topOrLeft.r+temp; i++){
            if(m_board[i][topOrLeft.c]=='.'){                //check if a ship can be added
                count++;
            }
        }
        if(count==m_game.shipLength(shipId)){
            for(int j=topOrLeft.r;j<topOrLeft.r+m_game.shipLength(shipId); j++){
                m_board[j][topOrLeft.c]=m_game.shipSymbol(shipId);
            }
            shiplist.push_back(shipId);
            return true;
        }
    }
    else if(dir==HORIZONTAL){
        if(topOrLeft.c+m_game.shipLength(shipId)>m_game.cols()-1){
            return false;
        }
        for(int m=topOrLeft.c;m<topOrLeft.c+m_game.shipLength(shipId); m++){
            if(m_board[topOrLeft.r][m]=='.'){                //check if a ship can be added
                count++;
            }
        }
        if(count==m_game.shipLength(shipId)){
            for(int n=topOrLeft.c;n<topOrLeft.c+m_game.shipLength(shipId); n++){
                m_board[topOrLeft.r][n]=m_game.shipSymbol(shipId);
            }
            shiplist.push_back(shipId);
            return true;
        }
    }
    
    return false;// This compiles, but may not be correct
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    if(shipId<0||shipId>=m_game.nShips()){
        return false;
    }
    if(dir==VERTICAL && m_board[topOrLeft.r+m_game.shipLength(shipId)-1][topOrLeft.c]==m_game.shipSymbol(shipId)){
        for(int i=topOrLeft.r;i<topOrLeft.r+m_game.shipLength(shipId); i++){
            m_board[i][topOrLeft.c]='.';
        }
        vector<int>::iterator a, b, it;
        a= shiplist.begin();
        b= shiplist.end();
        it = find(a, b, shipId);
        if(it!=b){
            shiplist.erase(it);
            return true;
        }
    }
    else if(dir==HORIZONTAL && m_board[topOrLeft.r][topOrLeft.c+m_game.shipLength(shipId)-1]==m_game.shipSymbol(shipId)){
        for(int m=topOrLeft.c;m<topOrLeft.c+m_game.shipLength(shipId); m++){
            m_board[topOrLeft.r][m]='.';
        }
        vector<int>::iterator c, d, ti;
        c= shiplist.begin();
        d= shiplist.end();
        ti = find(c, d, shipId);
        if(ti!=d){
            shiplist.erase(ti);
            return true;
        }
    }
    return false; // This compiles, but may not be correct
}

void BoardImpl::display(bool shotsOnly) const      //display the board
{
    cout<<"  ";
    for(int a=0;a<m_game.cols();a++){
        cout<<a;
    }
    cout<<endl;
    for(int i=0;i<m_game.rows();i++){
        cout<<i<<" ";
        for(int j=0; j<m_game.cols(); j++){
            if(!shotsOnly)
                cout<<m_board[i][j];
            else
                if(m_board[i][j]=='o' ||m_board[i][j]=='X')
                    cout<<m_board[i][j];
                else
                    cout<<'.';
        }
        cout<<endl;
    }
    // This compiles, but may not be correct
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)    //make an attack
{
    shotHit=false;
    shipDestroyed = false;
    char temp = '\0';
    if(p.r>=m_game.rows()||p.r<0)
        return false;
    else if(p.c>=m_game.cols()||p.c<0)
        return false;
    else if(m_board[p.r][p.c]=='o'||m_board[p.r][p.c]=='X'){
        return false;
    }
    else{
        if(m_board[p.r][p.c]=='.')
            m_board[p.r][p.c]='o';
        else{
            shipDestroyed = true;
            temp =m_board[p.r][p.c];
            shotHit=true;
            m_board[p.r][p.c]='X';
            for(int a=0;a<m_game.rows();a++){
                for(int b=0; b<m_game.cols();b++){
                    if(temp==m_board[a][b]){
                        shipDestroyed = false;
                    }
                }
            }
            if(shipDestroyed==true){
                for(int p=0;p<shiplist.size();p++){
                    if(temp==m_game.shipSymbol(shiplist[p])){
                        int tempship = shiplist[p];
                        vector<int>::iterator a, b, it;
                        a= shiplist.begin();
                        b= shiplist.end();
                        it = find(a, b, tempship);
                        if(it!=b){
                            shiplist.erase(it);
                            shipId = tempship;
                        }
                    }
                        
                }

            }
        }
        return true;
    }
    // This compiles, but may not be correct
}

bool BoardImpl::allShipsDestroyed() const        //see if all ships are destroyed
{
    int count = 0;
    for(int a=0;a<m_game.rows();a++){
        for(int b=0; b<m_game.cols();b++){
            if(m_board[a][b]=='X')
                count++;
        }
    }
    int total =0;
    for(int i=0; i<m_game.nShips();i++){
        total += m_game.shipLength(i);
    }
    if(count==total){
        return true;
    }
    return false;// This compiles, but may not be correct
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
