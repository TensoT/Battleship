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


bool nextDestroyShot(int size, int field[][10], int &x, int &y, int last_x, int last_y, int &destroyPhase, int &distance, bool reset = false){


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

    // State Memory/ destroy Cursor
    int destroyPhase;
    int distance;

    int last_x = -1;
    int last_y = -1;

    bool targetLocked = false;
    int hit1_x = -1;
    int hit1_y = -1;
    int hit2_x = -1;
    int hit2_y = -1;

    int dirPhaseA = -1;
    int dirPhaseB = -1;
    bool shootA = true;



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
            nextDestroyShot(size, field, x, y, last_x, last_y, int destroyPhase, int distance, true);

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
                if (!targetLocked){
                    haveTarget = nextDestroyShot(size, field, x, y, last_x, last_y);
                    }
                else {
                    int last_x = hit1_x;
                    int last_y = hit1_y;
                    if (shootA){
                        int phaseWanted = dirPhaseA;
                        }
                    else {
                        int phaseWanted = dirPhaseB;
                        }
                    int dx[] = {-1, 1, 0, 0};
                    int dy[] = {0, 0, -1, 1};

                    bool found = false;
                    int tmpDist = 1;

                    while(tmpDist <= size){
                        int tx = last_x + dx[phaseWanted]*tmpDist;
                        int ty = last_y + dy[phaseWanted]*tmpDist;

                        if(tx >=0 && tx < size && ty >= 0 && ty < size){
                            if(field[tx][ty] == 0){
                                x = tx;
                                y = ty;
                                found = true;
                                break;
                                }
                            }
                            tmpDist++;
                        }

                    if(found){
                        haveTarget = true;
                        shootA = !shootA;
                        }
                    else {
                        shootA = !shootA;
                        haveTarget = false;
                        }

                    }

                if(!haveTarget){
                    inDestroyMode = false;
                    targetLocked = false;
                    haveTarget = nextHuntShot(size, field, x, y);
                    }
                }
            if(!haveTarget){
                continue;
                }

            // Open Fire!

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
                        targetLocked = false;

                        hit1_x = x;
                        hit1_y = y;
                        hit2_x = -1;
                        hit2_y = -1;
                        dirPhaseA = -1;
                        dirPhaseB = -1;
                        shootA = true;

                        last_x = x;
                        last_y = y;

                        nextDestroyShot(size, field, x, y, last_x, last_y, destroyPhase, distance, true);
                    }
                    else {
                        if(!targetLocked){
                            hit2_x = x;
                            hit2_y = y;

                            if(hit2_x == hit1_x-1 && hit2_y == hit1_y){
                                dirPhaseA = 0;
                                dirPhaseB = 1;
                                targetLocked = true;
                                }
                            if(hit2_x == hit1_x+1 && hit2_y == hit1_y){
                                dirPhaseA = 1;
                                dirPhaseB = 0;
                                targetLocked = true;
                                }
                            if(hit2_x == hit1_x && hit2_y == hit1_y-1){
                                dirPhaseA = 2;
                                dirPhaseB = 3;
                                targetLocked = true;
                                }
                            if(hit2_x == hit1_x && hit2_y == hit1_y+1){
                                dirPhaseA = 3;
                                dirPhaseB = 2;
                                targetLocked = true;
                                }

                            if(!targetLocked){
                                last_x = x;
                                last_y = y;
                                nextDestroyShot(size, field, x, y, last_x, last_y, destroyPhase, distance, true);
                                }
                        }
                        else if(strcmp(response, "WATER") == 0){
                            if (inDestroyMode && targetLocked){
                                shootA = !shootA;
                                }
                            }
                        else if (strcmp(response, "SHIP_DESTROYED") == 0){
                            inDestroyMode = false;
                            last_x = -1;
                            last_y = -1;
                            targetLocked = false;
                            hit1_x = 0;
                            hit1_y = 0;
                            hit2_x = 0;
                            hit2_y = 0;
                            dirPhaseA = -1;
                            dirPhaseB = -1;
                            shootA = true;

                            nextHuntShot(size, field, x, y, true);
                            nextDestroyShot(size, field, x, y, last_x, last_y, destroyPhase, distance, true);
                                }
                            }


                    /**
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

