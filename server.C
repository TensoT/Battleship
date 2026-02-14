
/*

 * server

 *

 *  Created on: 13.02.2026

 *      Author: TensoT

 */

#include <cstdio> // standard input and output library
#include <cstdlib> // this includes functions regarding memory allocation
#include <cstring> // contains string functions
#include <cerrno> //It defines macros for reporting and retrieving error conditions through error codes
#include <ctime> //contains various functions for manipulating date and time

#include <unistd.h> //contains various constants
#include <sys/types.h> //contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h> // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses

#include "SIMPLESOCKET.H"

#include <string>
#include <iostream>
#include "TASK3.H"
#include "fstream"



/*Kindklasse von TCPserver : MySrv (benötigt Konstruktor MySrv(int,int) und #myResponse(string):string

*/

/**
 *
 *  \class MySrv
 *  \brief     The class defining our communication protocol
 *              (Server-Client communication)
 *
 */



using namespace std;

class MySrv : public TCPserver {
public:

    int count_shots;
    int count_games;
    int shots_used;

    MySrv(int port, int bsize) : TCPserver(port, bsize){};

    void newGame(){
    if (world_) {delete world_;
    }
    world_ = new TASK3::World();
    count_shots = 0;
    for (int y=0; y < 10; y++){
        for (int x=0; x < 10; x++){
            view_[y][x] = '_';
            }
        }
    }


protected:

    string myResponse(string input){
    char cmd[16];
    int x,y;


    if (sscanf(input.c_str(), "%15s", cmd) == 1){

    if (cmd[0] == 'Q' && cmd[1] == 'U' && cmd[2] == 'I' && cmd[3] == 'T'){
    return "BYEBYE";
    }
    if (cmd[0] == 'N' && cmd[1] == 'E' && cmd[2] == 'W' && cmd[3] == 'G' && cmd[4] == 'A' && cmd[5] == 'M' && cmd[6] == 'E'){
    newGame();
    return "NEW GAME STARTED\n" +boardString() + "\n";
    }
    if (sscanf(input.c_str(), "SHOT %d %d", &x, &y)== 2){
    TASK3::ShootResult res = world_->shoot(x,y);
    updateView(x, y, res);
    count_shots += 1;

    string result;
    switch (res) {
        case TASK3::WATER:
            result = "WATER";
            break;
        case TASK3::SHIP_HIT:
            result = "SHIP_HIT";
            break;
        case TASK3::SHIP_DESTROYED:
            result = "SHIP_DESTROYED";
            break;
        case TASK3::GAME_OVER:
            shots_used = count_shots;
            count_games += 1;
            result = "GAME_OVER \n\nShoots used: " + to_string(shots_used);
            break;
        default:
            result = "ERROR";
    }
    return result + "\n" + boardString() + "\n";
    //return result + "\n";
    }
    }
    return "UNKNOWN COMMAND\n";

    }



private:

    TASK3::World* world_ = nullptr;
    char view_[10][10];



    void updateView(int x, int y, TASK3::ShootResult res) {
        if(x < 1 || x > 10 || y < 1 || y > 10) return;
        if (res == TASK3::WATER){
        view_[y-1][x-1] = 'o';
        }
        else if (res == TASK3::SHIP_HIT ||
                 res == TASK3::SHIP_DESTROYED ||
                 res == TASK3::GAME_OVER) {
                 view_[y-1][x-1] = 'D';
                 }
        }

    string boardString(){
    string out;
    out = "\n";
    for (int y=0; y<10; y++){
        for (int x=0; x < 10; x++){
            out += view_[y][x];
            }
         out += "\n";
        }
    return out;
    }



};

int main(){
	srand(time(nullptr));
	 MySrv srv(2022,25);
	 srv.newGame();
	 srv.count_shots = 0;
	 srv.count_games = 0;
	 srv.run();


}



