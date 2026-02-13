

/*

 * client_pattern

 *

 *  Created on: 13.02.2026

 *      Author: TensoT

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



bool nextShot(int size, int field[][10], int &x, int &y, bool reset=false) {

// Hunt-Schachbrettmuster (kurze Erklärung):

// Ziel: Das Spielfeld systematisch durchsuchen, ohne jedes Feld direkt

// nacheinander zu prüfen. Das Schachbrettmuster lässt Zwischenräume

// zwischen getesteten Feldern, die die Wahrscheinlichkeit erhöhen, ein

// längeres Schiff zu treffen (da Schiffe mindestens 2 Felder lang sind).

//

// Vorgehen (in Phasen):

// - Phase 0: Schieße auf Felder einer Parität (z.B. (x+y) gerade)

// - Phase 1: Schieße auf Felder der anderen Parität (x+y ungerade)

// - Phase 2: Prüfe die vier Ecken des Spielfelds separat

//

// Implementation-Details:

// - `shotIndex` wird in ein (tx,ty) umgerechnet: tx = shotIndex/size,

//   ty = shotIndex%size. So wird ein Indexraum 0..size*size-1 über das

//   Board abgebildet.

// - Ecken werden übersprungen und erst in Phase 2 behandelt.

// - Ein Feld wird nur gewählt, wenn `field[tx][ty] == 0` (noch nicht geschossen).

//


    int shotIndex = 0;

    int phase = 0;



    if (reset){

        shotIndex = 0;

        phase = 0;

        return false;

        }

    while (true){

        // Phase 2: Ecken prüfen (werden separat behandelt)

        if (phase == 2){

            int corners[4][2] ={

            {0,0},{0, size-1},{size-1,0},{size-1,size-1}};

            if (shotIndex >= 4){

                return false;}

            x = corners[shotIndex][0];

            y = corners[shotIndex][1];

            shotIndex++;

            if(field[x][y] == 0){

            return true;}

            continue;

            }



        if (shotIndex >= size*size){

            shotIndex = 0;

            phase++;

            continue;

            }



        // Index -> Koordinaten (zeilenbasiert):

        // tx = shotIndex / size  (Zeile), ty = shotIndex % size (Spalte)

        int tx = shotIndex / size;

        int ty = shotIndex % size;

        shotIndex++;



        // Ecken überspringen; sie werden in Phase 2 behandelt

        bool corner = ( (tx==0 && ty==0) ||

                (tx==0 && ty==size-1) ||

                (tx==size-1 && ty==0) ||

                (tx==size-1 && ty==size-1) );

        if (corner){

        continue;}



        // Paritäts-Check: in Phase 0 nur eine Parität wählen, in Phase 1 die andere

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



int main() {


// setup


    int nmb_of_games;

    nmb_of_games = 100;

// startsequenz

    srand(time(NULL));

    TCPclient c;

    string host = "127.0.0.1";



    //connect to host

    c.conn(host , 2022);



    bool goOn=1;





    string msg_recieved;

    string msg;

    char response[16] = {0};



    const int size = 10;

    int field [size][size] ={0};



    int count_shots = 0;

    int x, y;



    msg = "NEWGAME\n";

    //cout << "client sends:\n" << msg << endl;

                c.sendData(msg);

                sleep(1);

                msg_recieved = c.receive(200);

                //cout << "got response:\n" << msg_recieved << endl;

                sleep(1);



    while (goOn){



        if (sscanf(msg_recieved.c_str(), "%15s", response) == 1){



        if (response[0] == 'G' && response[1] == 'A' && response[2] == 'M' && response[3] == 'E' && response[4] == '_' && response[5] == 'O' && response[6] == 'V'&& response[7] == 'E' && response[8] == 'R'){

        count_games ++;



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

            nextShot(size, field, x,y, true);

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



            if (nextShot(size, field,x,y)){



            if (field[x][y] == 1){

                continue;

                }

            else {

                msg = "SHOT " + to_string(x+1) + " " + to_string(y+1) + "\n";

                //cout << "client sends:\n" << msg << endl;

                count_shots ++;

                field[x][y] = 1;

                c.sendData(msg);

                sleep(0);

                msg_recieved = c.receive(200);

                //cout << "got response:\n" << msg_recieved << endl;

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

}

