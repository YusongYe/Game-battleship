#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
: Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
    // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
    // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
    // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class HumanPlayer : public Player
{
public:
    HumanPlayer(string nm, const Game& g);
    virtual bool isHuman() const { return true; };
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId) ;
    virtual void recordAttackByOpponent(Point p) ;
    
private:
    Direction placedir(int id);
    void placept(int id, Board& b, Direction d);
};

HumanPlayer::HumanPlayer(string nm, const Game& g)
:Player(nm, g)
{}

bool HumanPlayer::placeShips(Board& b){           //place all the ships
    Direction dir;
    cout<<"Shuman the Human must place "<<game().nShips()<<" ships."<<endl;
    b.display(false);
    for(int i=0;i<game().nShips();i++){
        dir = placedir(i);
        placept(i, b, dir);
        b.display(false);
    }
    return true;
}

Direction HumanPlayer::placedir(int id){           //let user input direction
    string temp;
    cout<<"Enter h or v for direction of "<<game().shipName(id)<<" (length "<<game().shipLength(id)<<"):";
    getline(cin,temp);
    while(temp!="h" &&temp!="v"){
        cout<<"Direction must be h or v."<<endl;
        cout<<"Enter h or v for direction of "<<game().shipName(id)<<" (length "<<game().shipLength(id)<<"):";
        getline(cin,temp);
    }
    
    if(temp=="h"){
        return HORIZONTAL;
    }
    else{
        return VERTICAL;
    }
}

void HumanPlayer::placept(int id, Board& b,Direction d){     //let user input ship point
    int tempx;
    int tempy;
    cout<<"Enter row and column of leftmost cell (e.g. 3 5):";
    
    while(!getLineWithTwoIntegers(tempx, tempy)){
        cout<<"You must enter two integers."<<endl;
        cout<<"Enter row and column of leftmost cell (e.g. 3 5):";
        //getLineWithTwoIntegers(tempx, tempy);
    }
    while(!b.placeShip(Point(tempx,tempy),id, d)){
        cout<<"The ship can not be placed there."<<endl;
        cout<<"Enter row and column of leftmost cell (e.g. 3 5):";
        //getLineWithTwoIntegers(tempx, tempy);
        while(!getLineWithTwoIntegers(tempx, tempy)){
            cout<<"You must enter two integers."<<endl;
            cout<<"Enter row and column of leftmost cell (e.g. 3 5):";
            //getLineWithTwoIntegers(tempx, tempy);
        }
    }
    b.placeShip(Point(tempx,tempy),id, d);
}

Point HumanPlayer::recommendAttack(){
    int r=0;
    int c=0;
    cout<<"Enter the row and column to attack (e.g, 3 5): ";
    if(!getLineWithTwoIntegers(r,c)){
        cout<<"You must enter two integers."<<endl;
        return recommendAttack();
    }
    return Point(r,c);
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                     bool shipDestroyed, int shipId) {
    
}

void HumanPlayer:: recordAttackByOpponent(Point p) {
    
}


//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer : public Player
{
public:
    MediocrePlayer(string nm, const Game& g);
    virtual bool isHuman() const { return false; };
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    int state;
    vector <Point> visited;
    vector <Point> potentialpoints;
    vector <Point> shipplaced;
    Point notvisited();
    bool helperplace(Board& b, int cur, int num);
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g)
:Player(nm, g)
{
    state=1;
}

bool MediocrePlayer::placeShips(Board& b){        //place all the ships
    for(int i=0; i<50; i++){
        b.block();
        if(helperplace(b, 0, game().nShips())){
            b.unblock();
            return true;
        }
        b.unblock();
    }
    return false;
}

bool MediocrePlayer::helperplace(Board& b, int cur, int num){          //help place the ships
    for(int i=0; i<game().rows();i++){
        for(int j =0; j<game().cols(); j++){
            Point temp(i,j);
            if(b.placeShip(temp,cur,HORIZONTAL)||b.placeShip(temp,cur,VERTICAL)){
                shipplaced.push_back(temp);
                if(cur==num-1){
                    return true;
                }
                else if(helperplace(b,cur+1,num)){
                    return true;
                }
                else{
                    //cur =cur-1;
                    shipplaced.pop_back();
                    if(!b.unplaceShip(temp,cur,HORIZONTAL)){
                        b.unplaceShip(temp,cur,VERTICAL);
                    }
                }
            }
        }
    }
    return false;
}


Point MediocrePlayer::notvisited(){             //random return a point that is not visited
    Point p = game().randomPoint();
    for(int a=0;a<visited.size();a++){
        if(visited[a].r==p.r && visited[a].c==p.c){
            return notvisited();
        }
    }
    return p;
}

Point MediocrePlayer::recommendAttack(){          
    if (state==1){
        Point p = notvisited();
        visited.push_back(p);
        return p;
    }
    else{    //state ==2
        int tempsize = potentialpoints.size();
        int temp = randInt(tempsize);
        Point temppoint = potentialpoints[temp];
        for(int a=0;a<potentialpoints.size();a++){
            if(potentialpoints[a].r==temppoint.r && potentialpoints[a].c==temppoint.c){
                potentialpoints.erase(potentialpoints.begin()+a);
            }
        }
        visited.push_back(temppoint);
        return temppoint;
    }
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){
    if(state==1){
        if(shotHit&&!shipDestroyed){
            for(int i=1;i<=4; i++){
                Point temp1 (p.r,p.c-i);
                
                if(game().isValid(temp1)){
                    potentialpoints.push_back(temp1);
                    for(int n=0;n<visited.size();n++){
                        if(visited[n].r==temp1.r && visited[n].c==temp1.c){
                            potentialpoints.pop_back();
                        }
                    }
                }
            }
            for(int i=1;i<=4; i++){
                Point temp2 (p.r-i,p.c);
                
                if(game().isValid(temp2)){
                    potentialpoints.push_back(temp2);
                    for(int n=0;n<visited.size();n++){
                        if(visited[n].r==temp2.r && visited[n].c==temp2.c){
                            potentialpoints.pop_back();
                        }
                    }
                }
            }
            for(int i=1;i<=4; i++){
                Point temp3 (p.r,p.c+i);
                
                if(game().isValid(temp3)){
                    potentialpoints.push_back(temp3);
                    for(int n=0;n<visited.size();n++){
                        if(visited[n].r==temp3.r && visited[n].c==temp3.c){
                            potentialpoints.pop_back();
                        }
                    }
                }
            }
            for(int i=1;i<=4; i++){
                Point temp4 (p.r+i,p.c);
                
                if(game().isValid(temp4)){
                    potentialpoints.push_back(temp4);
                    for(int n=0;n<visited.size();n++){
                        if(visited[n].r==temp4.r && visited[n].c==temp4.c){
                            potentialpoints.pop_back();
                        }
                    }
                }
            }
            state =2;
        }
    }
    else{
        if(shotHit&&shipDestroyed){
            while(!potentialpoints.empty()){
                potentialpoints.pop_back();
            }
            state =1;
        }
    }
}

void MediocrePlayer::recordAttackByOpponent(Point p) {
    
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.

// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.


//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer : public Player
{
public:
    GoodPlayer(string nm, const Game& g);
    virtual bool isHuman() const { return false; };
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    int state;
    vector <Point> visited;
    vector <Point> potentialpoints;
    vector <Point> shipplaced;
    vector <Point> potentialdir;
    vector <Point> destroyed;
    vector <Point> wasted;
    Point notvisited();
    Point center;
    Point previous;
    bool isnotvisited(Point a);
    bool isdestroyed(Point a);
    bool iswasted(Point a);
    bool helperplace(Board& b, int cur, int num);
};

GoodPlayer::GoodPlayer(string nm, const Game& g)
:Player(nm,g)
{
    state =1;
}

bool GoodPlayer::placeShips(Board& b){
    for(int i=0; i<50; i++){
        b.block();
        if(helperplace(b, 0, game().nShips())){
            b.unblock();
            return true;
        }
        b.unblock();
    }
    return false;
}

bool GoodPlayer::helperplace(Board& b, int cur, int num){
    for(int i=0; i<game().rows();i++){
        for(int j =0; j<game().cols(); j++){
            Point temp(i,j);
            if(b.placeShip(temp,cur,HORIZONTAL)||b.placeShip(temp,cur,VERTICAL)){
                shipplaced.push_back(temp);
                if(cur==num-1){
                    return true;
                }
                else if(helperplace(b,cur+1,num)){
                    return true;
                }
                else{
                    //cur =cur-1;
                    shipplaced.pop_back();
                    if(!b.unplaceShip(temp,cur,HORIZONTAL)){
                        b.unplaceShip(temp,cur,VERTICAL);
                    }
                }
            }
        }
    }
    return false;
}


Point GoodPlayer::notvisited(){
    
    int randomtime =0;
    Point p = game().randomPoint();
    while((!isnotvisited(p)||(p.r+p.c)%2==1)&&randomtime<100){
        p =game().randomPoint();
        randomtime++;
    }
    if(isnotvisited(p)){
        return p;
    }
    else{
        while(!isnotvisited(p)){
            p = game().randomPoint();
        }
        return p;
    }
    
}

bool GoodPlayer::isnotvisited(Point a){
    for(int i=0;i<visited.size();i++){
        if(visited[i].r==a.r && visited[i].c==a.c){
            return false;
        }
    }
    return true;
}

bool GoodPlayer::isdestroyed(Point a){
    for(int i=0;i<destroyed.size();i++){
        if(destroyed[i].r==a.r && destroyed[i].c==a.c){
            return true;
        }
    }
    return false;
}

bool GoodPlayer::iswasted(Point a){
    for(int i=0;i<wasted.size();i++){
        if(wasted[i].r==a.r && wasted[i].c==a.c){
            return true;
        }
    }
    return false;
}

Point GoodPlayer::recommendAttack(){
    if (state==1){
        Point p = notvisited();
        visited.push_back(p);
        return p;
    }
    else if(state==2){    //state ==2
        int tempsize = potentialpoints.size();
        int temp = randInt(tempsize);
        Point temppoint = potentialpoints[temp];
        for(int a=0;a<potentialpoints.size();a++){
            if(potentialpoints[a].r==temppoint.r && potentialpoints[a].c==temppoint.c){
                potentialpoints.erase(potentialpoints.begin()+a);
            }
        }
        visited.push_back(temppoint);
        return temppoint;
    }
    else{
        Point temppoint = potentialdir[0];
        potentialdir.erase(potentialdir.begin());
        visited.push_back(temppoint);
        return temppoint;
    }
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){
    if(shotHit){
        destroyed.push_back(p);
    }
    if(!shotHit){
        wasted.push_back(p);
    }
    if(state==1){
        if(shotHit&&!shipDestroyed){
            Point temp1(p.r,p.c-1);
            if(isnotvisited(temp1)&&game().isValid(temp1)){
                potentialpoints.push_back(temp1);
            }
            else if(isdestroyed(temp1)){
                int i=1;
                while(i<=3){
                    if(!game().isValid(Point(temp1.r,temp1.c-i))){
                        i=4;
                    }
                    else if(iswasted(Point(temp1.r,temp1.c-i))){
                        i=4;
                    }
                    else if(isdestroyed(Point(temp1.r,temp1.c-i))){
                        i++;
                    }
                    else{     //is not visited
                        potentialpoints.push_back(Point(temp1.r,temp1.c-i));
                        i=4;
                    }
                }
            }
            
            Point temp2(p.r,p.c+1);
            if(isnotvisited(temp2)&&game().isValid(temp2)){
                potentialpoints.push_back(temp2);
            }
            else if(isdestroyed(temp2)){
                int i=1;
                while(i<=3){
                    if(!game().isValid(Point(temp2.r,temp2.c+i))){
                        i=4;
                    }
                    else if(iswasted(Point(temp2.r,temp2.c+i))){
                        i=4;
                    }
                    else if(isdestroyed(Point(temp2.r,temp2.c+i))){
                        i++;
                    }
                    else{     //is not visited
                        potentialpoints.push_back(Point(temp2.r,temp2.c+i));
                        i=4;
                    }
                }
            }
            
            Point temp3(p.r-1,p.c);
            if(isnotvisited(temp3)&&game().isValid(temp3)){
                potentialpoints.push_back(temp3);
            }
            else if(isdestroyed(temp3)){
                int i=1;
                while(i<=3){
                    if(!game().isValid(Point(temp3.r-i,temp3.c))){
                        i=4;
                    }
                    else if(iswasted(Point(temp3.r-i,temp3.c))){
                        i=4;
                    }
                    else if(isdestroyed(Point(temp3.r-i,temp3.c))){
                        i++;
                    }
                    else{     //is not visited
                        potentialpoints.push_back(Point(temp3.r-i,temp3.c));
                        i=4;
                    }
                }
            }
            
            Point temp4(p.r+1,p.c);
            if(isnotvisited(temp4)&&game().isValid(temp4)){
                potentialpoints.push_back(temp4);
            }
            else if(isdestroyed(temp4)){
                int i=1;
                while(i<=3){
                    if(!game().isValid(Point(temp4.r+i,temp4.c))){
                        i=4;
                    }
                    else if(iswasted(Point(temp4.r+i,temp4.c))){
                        i=4;
                    }
                    else if(isdestroyed(Point(temp4.r+i,temp4.c))){
                        i++;
                    }
                    else{     //is not visited
                        potentialpoints.push_back(Point(temp4.r+i,temp4.c));
                        i=4;
                    }
                }
            }
            center =p;
            state=2;
        }
    }
    else if(state==2){
        if(shotHit&&shipDestroyed){
            while(!potentialpoints.empty()){
                potentialpoints.pop_back();
            }
            state =1;
        }
        
        else if(shotHit&&!shipDestroyed){
            previous=p;
            if(center.r==previous.r){
                if(center.c>previous.c){
                    int i=1;
                    while(game().isValid(Point(center.r,previous.c-i))){
                        if(game().isValid(Point(center.r,previous.c-i))&&isnotvisited(Point(center.r,previous.c-i))){
                            potentialdir.push_back(Point(center.r,previous.c-i));
                        }
                        i++;
                    }
                }
                else{
                    int i=1;
                    while(game().isValid(Point(center.r,previous.c+i))){
                        if(game().isValid(Point(center.r,previous.c+i))&&isnotvisited(Point(center.r,previous.c+i))){
                            potentialdir.push_back(Point(center.r,previous.c+i));
                        }
                        i++;
                    }
                }
                
            }
            
            else{     //center.c==previous.c
                if(center.r>previous.r){
                    int i=1;
                    while(game().isValid(Point(previous.r-i,previous.c))){
                        if(game().isValid(Point(previous.r-i,previous.c))&&isnotvisited(Point(previous.r-i,previous.c))){
                            potentialdir.push_back(Point(previous.r-i,previous.c));
                        }
                        i++;
                    }
                }
                else{
                    int i=1;
                    while(game().isValid(Point(previous.r+i,previous.c))){
                        if(game().isValid(Point(previous.r+i,previous.c))&&isnotvisited(Point(previous.r+i,previous.c))){
                            potentialdir.push_back(Point(previous.r+i,previous.c));
                        }
                        i++;
                    }
                }
            }
            if(potentialdir.empty()){
                state=2;
            }
            else{
                state =3;
            }
        }
        
        else{
            if(potentialpoints.empty()){
                state =1;
            }
            else{
                state=2;
            }
        }
    }
    else{                  //state==3
        if(shotHit&&!shipDestroyed){
            if(potentialdir.empty()){
                state =2;
            }
        }
        else if (shotHit&&shipDestroyed){
            while(!potentialdir.empty()){
                potentialdir.pop_back();
            }
            while(!potentialpoints.empty()){
                potentialpoints.pop_back();
            }
            state=1;
        }
        else{
            state=2;
            while(!potentialdir.empty()){
                potentialdir.pop_back();
            }
        }
    }
}


void GoodPlayer::recordAttackByOpponent(Point p){
    
}




// TODO:  You need to replace this with a real class declaration and
//        implementation.

//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
         type != types[pos]; pos++)
        ;
    switch (pos)
    {
        case 0:  return new HumanPlayer(nm, g);
        case 1:  return new AwfulPlayer(nm, g);
        case 2:  return new MediocrePlayer(nm, g);
        case 3:  return new GoodPlayer(nm, g);
        default: return nullptr;
    }
}
