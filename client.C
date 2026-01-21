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

int main() {
	srand(time(NULL));
	TCPclient c;
	string host = "127.0.0.1";

	//connect to host
	c.conn(host , 2022);

	//int i=0;
	bool goOn=1;

	int nmb_of_games, count_games;
    nmb_of_games = 10;
    count_games = 0;

    string msg_recieved;
    string msg;
    char response[16] = {0};

    int size = 10;
    int field [size][size] ={0};

    int count_shots = 0;
    int x, y;



    msg = "NEWGAME\n";
    cout << "client sends:\n" << msg << endl;
                c.sendData(msg);
                sleep(1);
                msg_recieved = c.receive(200);
                cout << "got response:\n" << msg_recieved << endl;
                sleep(1);


	while (goOn){

        if (sscanf(msg_recieved.c_str(), "%15s", response) == 1){


        if (response[0] == 'G' && response[1] == 'A' && response[2] == 'M' && response[3] == 'E' && response[4] == '_' && response[5] == 'O' && response[6] == 'V'&& response[7] == 'E' && response[8] == 'R'){
        count_games ++;

        if(nmb_of_games <= count_games){
			msg = string("BYEBYE");
			c.sendData(msg);
			goOn = 0;
			break;
			}
        else {
            msg = string ("NEWGAME\n");

            count_shots = 0;
            for (int i = 0; i < size; i++){
                for(int j = 0; j< size; j++){
                    field[i][j] = 0;
                    }
                }
             c.sendData(msg);
             sleep(1);
                msg_recieved = c.receive(200);
                cout << "got response:" << msg_recieved << endl;
                sleep(0);
            continue;
            }
            }



        else if (count_shots < size * size){

            x = rand() % size;
            y = rand() % size;

            if (field[x][y] == 1){
                continue;
                }
            else {
                msg = "SHOT " + to_string(x+1) + " " + to_string(y+1) + "\n";
                cout << "client sends:\n" << msg << endl;
                count_shots ++;
                field[x][y] = 1;
                c.sendData(msg);
                sleep(0);
                msg_recieved = c.receive(200);
                cout << "got response:\n" << msg_recieved << endl;
                sleep(0);}
            }}
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


    /*    int size;
        size = 10;
        int field [][] ={0};
        int count_shots = 0;
        int x, y;
        int sent [10][10];



        int nmb_of_games, count_games;
        nmb_of_games = 10;
        count_games = 0;

    while (goOn){
        if((nmb_of_games = count_games){
			msg = string("BYEBYE");
			goOn = 0;
			break;
			}
        else if (msg_recieved = string ("GAME_OVER")){
            msg = string ("NEW GAME");
             c.sendData(msg);
            continue;
            }
        else if (count_shots < size * size){
            int x,y;
            string msg;
            x = rand() % size;
            y = rand() % size;

            if (field[x][y] = 1){
                continue;
                }
            else {
                msg = "SHOT %d %d", (x+1), (y+1);}
                cout << "client sends:" << msg << endl;
                c.sendData(msg);
                sleep(1);
                msg_recieved = c.receive(200);
                cout << "got response:" << msg << endl;
                sleep(1);
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


        */



