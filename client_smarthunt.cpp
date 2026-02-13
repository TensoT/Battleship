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
                            (x==size-1 && y==size-1) );
                            }

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

        //DESTROY

enum Axis {NONE, NS, EW};

struct DestroyState{

    bool active;
    int hitCount;
    int x1, y1, x2, y2;
    Axis axis;
    int stepPos, stepNeg;
    bool TryPositive;
};

void resetDestroy(DestroyState &ds){

    ds.active = false;
    ds.hitCount = 0;
    ds.axis = NONE;
    ds.stepPos = 1;
    ds.stepNeg = 1;
    ds.TryPositive = true;
    }

bool inBounds(int x, int y, int size){
    return(x >= 0 && x < size && y >=0 && y <size);
    }

void updateSegmentEnds (DestroyState &ds, int lastX, int lastY){
    if(ds.axis == EW){
        if(lastY < ds.y1){
            ds.y1 = lastY;
            ds.x1 = lastX;
            }
        if(lastY > ds.y2){
            ds.y2 = lastY;
            ds.x2 = lastX;
            }
        }
    if(ds.axis == NS){
        if(lastX < ds.x1){
            ds.y1 = lastY;
            ds.x1 = lastX;
            }
        if(lastX > ds.x2){
            ds.y2 = lastY;
            ds.x2 = lastX;
            }
        }
    }



bool nextDestroyShot(int size, int field[][10], DestroyState &ds, int &x, int &y){

    if(!ds.active) {
    return false;
    }

    if(ds.hitCount==1){
        int dx[4]={-1,1,0,0};
        int dy[4]={0,0,-1,1};
        for(int i=0; i<4;i++){
            int tx = ds.x1 + dx[i];
            int ty = ds.y1 + dy[i];
            if(inBounds(tx,ty,size) && field[tx][ty] == 0){
                x = tx;
                y = ty;
                return true;
                }
            }
            return false;
        }

    // axis-mode
    for(int tries=0; tries<2; tries++){
        bool pos = ds.TryPositive;
        int row, col;
        ds.TryPositive = !ds.TryPositive;
        if(ds.axis == NS){
             row = ds.x1;
            if (pos){
                col = ds.y2 + ds.stepPos;
                }
            else {
                 col = ds.y1 - ds.stepNeg;
                }
            //int col = pos ? ds.y2 + ds.stepPos : ds.y1 . ds.stepNeg;

            if (inBounds(row, col, size) && field[row][col] == 0){
                x = row;
                y = col;
                return true;
                }
            else {
                if (pos){
                    ds.stepPos++;}
                else {
                    ds.stepNeg++;}
                    }
            }
        else if(ds.axis == EW){
             col = ds.y1;
            if (pos){
                 row = ds.x2 + ds.stepPos;
                }
            else {
                 row = ds.x1 - ds.stepNeg;
                }
            //int row = pos ? ds.x2 + ds.stepPos : ds.x1 . ds.stepNeg;

            if (inBounds(row, col, size) && field[row][col] == 0){
                x = row;
                y = col;
                return true;
                }
            else {
                if (pos){
                    ds.stepPos++;}
                else {
                    ds.stepNeg++;}
                    }
        }
    }

    return false;
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
    nmb_of_games = 100;
    count_games = 0;

    int destroyPhase = 0;
    int distance = 1;

    string msg_recieved;
    string msg;
    char response[16] = {0};

    const int size = 10;
    int field [size][size] ={0};

    int count_shots = 0;
    int x, y;

    // State for Hunt/Destroy Phase

    int lastX;
    int lastY;

    lastX = -1;
    lastY = -1;

    DestroyState ds;
    resetDestroy(ds);


    // Kommunikationsaufbau

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
            nextHuntShot(size, field, x,y, true);
            resetDestroy(ds);

            // clear field from shots
            for (int i = 0; i < size; i++){
                for(int j = 0; j< size; j++){
                    field[i][j] = 0;
                    }
                }
             c.sendData(msg);
             sleep(0);
                msg_recieved = c.receive(200);
                //cout << "got response:" << msg_recieved << endl;
                sleep(0);
            continue;
            }
            }



        else if (count_shots < size * size){

            if (strcmp(response, "SHIP_DESTROYED") == 0){
                resetDestroy(ds);
                }

            else if (strcmp(response, "SHIP_HIT") == 0){

            if(!ds.active){
                ds.active = true;
                ds.hitCount = 1;
                ds.x1 = lastX;
                ds.y1 = lastY;
                }
            else if (ds.hitCount == 1){
                ds.hitCount = 2;
                ds.x2 = lastX;
                ds.y2 = lastY;

                if (ds.x1 == ds.x2){
                    ds.axis = EW;
                    }
                else if (ds.y1 == ds.y2){
                    ds.axis = NS;
                    }
                ds.stepPos = 1;
                ds.stepNeg = 1;
                ds.TryPositive = true;
                }
             else {
                updateSegmentEnds(ds, lastX, lastY);
                }
            }


        // Ziel bestimmen

        bool haveShot = false;

        if(ds.active){
            haveShot = nextDestroyShot(size, field, ds, x,y);
            if(!haveShot){
                resetDestroy(ds);
                }
            }

        if(!haveShot){
            if(!nextHuntShot(size, field, x, y)) {
            break;}
            }

        // Schießen

        msg = "SHOT " + to_string(x+1) + " " + to_string(y+1) + "\n";
                //cout << "client sends:\n" << msg << endl;
                count_shots ++;
                field[x][y] = 1;
                lastX = x;
                lastY = y;

                c.sendData(msg);
                sleep(0);
                msg_recieved = c.receive(200);

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


