/*
 * client.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */

#include <string>
#include <iostream>
#include <unistd.h> //contains various constants

#include "SIMPLESOCKET.H"


//new
#include <cstdio> // standard input and output library
#include <cstdlib> // this includes functions regarding memory allocation
#include <cstring> // contains string functions
#include <cerrno> //It defines macros for reporting and retrieving error conditions through error codes
#include <ctime>

using namespace std;

 bool is_corner(int x, int y, int size) {
                    return ( (x==0 && y==0) ||
                    (x==0 && y==size-1) ||
                    (x==size-1 && y==0) ||
                    (x==size-1 && y==size-1) );}

bool nextHuntShot(int size, int field[][10], int &x, int &y, bool reset=false){

        static int shotIndex =0;
        static int phase =0;

        if(reset){
            shotIndex = 0;
            phase = 0;
            return false;
            }

        while(true){
        if(phase == 2){
            int corners[4][2] ={
            {0,0},{0, size-1},{size-1,0},{size-1,size-1}};
            if (shotIndex >= 4){
                return false;}
            int tx = corners[shotIndex][0];
            int ty = corners[shotIndex][1];
            shotIndex++;
            if(field[tx][ty] == 0){
            x = tx;
            y = ty;
            return true;}
            continue;}

        if (shotIndex >= size*size){
            shotIndex = 0;
            phase++;
            continue;
            }

        int tx = shotIndex / size;
        int ty = shotIndex % size;
        shotIndex++;

         if (is_corner(tx, ty, size)){
        continue;}

        if (phase == 0 && (tx + ty) % 2 !=0){
        continue;}
        if (phase == 1 && (tx + ty) % 2 !=1){
        continue;}

        if(field[tx][ty] == 0){
            x = tx;
            y = ty;
            return true;}
        }
    }
// DESTROY PLAN

//enum LastPlan { PLAN_NONE, PLAN_NEIGHBOUR}

bool nextDestroyShot(int size, int field[][10], int &x, int &y, int last_x, int last_y, bool reset = false){

    static int destroyPhase = 0;
    static int distance = 1;
    if(reset){
        destroyPhase = 0;
        distance = 1;
        return false;
        }
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    while(true){
        if(destroyPhase >= 4){
            distance++;
            if(distance > size){
                return false;
                }
            destroyPhase=0;
            }
        int tx = last_x + dx[destroyPhase] * distance;
        int ty = last_y + dy[destroyPhase] * distance;
        destroyPhase++;

        if(tx<0 || tx >= size || ty<0 || ty >= size){
            continue;}

        if(field[tx][ty] == 0){
            x = tx;
            y = ty;
            return true;}
            }
    }




    //Programmstart
int main() {
    srand(time(NULL));
    TCPclient c;
    string host = "127.0.0.1";

    //connect to host
    c.conn(host , 2022);


    bool goOn=1;

    int nmb_of_games, count_games;
    nmb_of_games = 1000;
    count_games = 0;

    string msg_recieved;
    string msg;
    char response[16] = {0};

    const int size = 10;
    int field [size][10] = {0};

    int count_shots = 0;
    int x, y;

    // State for Hunt/Destroy Phase
    bool inDestroyMode = false;
    int last_x = -1;
    int last_y = -1;



    msg = "NEWGAME\n";
    //cout << "client sends:\n" << msg << endl;
                c.sendData(msg);
                sleep(1);
                msg_recieved = c.receive(200);
                //cout << "got response:\n" << msg_recieved << endl;
                sleep(1);


    while (goOn){

        if (sscanf(msg_recieved.c_str(), "%15s", response) != 1){
            continue;
            }


        //if (msg_recieved.compare("GAME_OVER") == 0){
        if (strcmp(response, "GAME_OVER") == 0){
        count_games ++;
        // response[0] == 'G' && response[1] == 'A' && response[2] == 'M' && response[3] == 'E' && response[4] == '_' && response[5] == 'O' && response[6] == 'V'&& response[7] == 'E' && response[8] == 'R'){


        if(nmb_of_games <= count_games){
            cout << to_string(count_shots) << endl;
            msg = string("BYEBYE");
            c.sendData(msg);
            goOn = 0;
            break;
            }
        else {
            msg = string ("NEWGAME\n");
            cout << to_string(count_shots) << endl;
            count_shots = 0;
            // clear field from shots
            for (int i = 0; i < size; i++){
                for(int j = 0; j< size; j++){
                    field[i][j] = 0;
                    }
                }
            inDestroyMode = false;
            last_x = -1;
            last_y = -1;
            nextHuntShot(size, field, x, y, true);
            nextDestroyShot(size, field, x, y, last_x, last_y, true);

             c.sendData(msg);
             sleep(0);
                msg_recieved = c.receive(200);
                //cout << "got response:" << msg_recieved << endl;
                sleep(0);
            continue;
            }
            }



        else if (count_shots < size * size){

            bool haveTarget = false;

            if(!inDestroyMode){
                haveTarget = nextHuntShot(size, field, x, y);
                }
            else {
                haveTarget = nextDestroyShot(size, field, x, y, last_x, last_y);
                if (!haveTarget){
                    inDestroyMode = false;
                    haveTarget = nextHuntShot(size, field, x, y);
                    }
                }

            if(!haveTarget){
                continue;}



                msg = "SHOT " + to_string(x+1) + " " + to_string(y+1) + "\n";
                //cout << "client sends:\n" << msg << endl;
                count_shots ++;
                field[x][y] = 1;

                c.sendData(msg);
                sleep(0);
                msg_recieved = c.receive(200);
                //cout << "got response:\n" << msg_recieved << endl;
                sleep(0);

                sscanf(msg_recieved.c_str(), "%15s", response);

            if(strcmp(response, "SHIP_HIT") == 0){
                if(!inDestroyMode){
                    inDestroyMode = true;
                    last_x = x;
                    last_y = y;
                    nextDestroyShot(size, field, x, y, last_x, last_y, true);
                    }
                }
            else if (strcmp(response, "SHIP_DESTROYED") == 0){
                inDestroyMode = false;
                last_x = -1;
                last_y = -1;
                nextHuntShot(size, field, x, y, true);}

            }

            else {
                msg = "ERROR";
                cout << "client sends:" << msg << endl;
                c.sendData(msg);
                sleep(1);
                msg_recieved = c.receive(200);
                cout << "got response:" << msg << endl;
                sleep(1);
            }

        }

}

