

/*

 * client_hunt

 *

 *  Created on: 13.02.2026

 *      Author: TensoT

 */



#include <string>

#include <iostream>

#include <unistd.h> //contains various constants



#include "SIMPLESOCKET.H"



// Standard C++ Bibliotheken für Ein-/Ausgabe, Speicherverwaltung und String-Operationen

#include <cstdio> // standard input and output library

#include <cstdlib> // this includes functions regarding memory allocation

#include <cstring> // contains string functions

#include <cerrno> // It defines macros for reporting and retrieving error conditions through error codes

#include <ctime>   // Für Zeitmessungen und Zufall



using namespace std;



// Prüfe ob Koordinaten eine Ecke des Spielfelds sind

bool is_corner(int x, int y, int size) {

    return ( (x==0 && y==0) ||

             (x==0 && y==size-1) ||

             (x==size-1 && y==0) ||

             (x==size-1 && y==size-1) );

}



/**

 * HUNT PHASE - Intelligente Schußstrategie zum Auffinden von Schiffen

 *

 * Diese Funktion verwendet ein Schachbrettmuster, um das Spielfeld effizient zu durchsuchen.

 * Sie durchläuft zwei Phasen:

 *  - Phase 0: Schießt auf Felder, wo (x+y) ungerade ist

 *  - Phase 1: Schießt auf Felder, wo (x+y) gerade ist

 *  - Phase 2: Schießt auf die vier Ecken

 *

 * Dies maximiert die Chancen, ein Schiff zu treffen, bevor die Destroy-Phase aktiviert wird.

 *

 * @param size Größe des Spielfelds

 * @param field Array für getätigte Schüsse

 * @param x Ausgabe: X-Koordinate des nächsten Schusses

 * @param y Ausgabe: Y-Koordinate des nächsten Schusses

 * @param reset Wenn true, wird die Hunt-Phase zurückgesetzt

 * @return true wenn ein Schuß möglich ist, false wenn Hunt-Phase beendet

 */

bool nextHuntShot(int size, int field[][10], int &x, int &y, bool reset=false){



    // Statische Variablen behalten ihren Zustand über mehrere Funktionsaufrufe

    static int shotIndex = 0;  // Index des aktuellen Schusses

    static int phase = 0;      // Aktuelle Phase (0, 1 oder 2)



    // Zurücksetzen der Hunt-Phase

    if(reset){

        shotIndex = 0;

        phase = 0;

        return false;

    }



    // Hauptschleife zum Finden des nächsten gültigen Schusses

    while(true){

        // Phase 2: Schießt auf die Ecken des Spielfelds

        if(phase == 2){

            int corners[4][2] = {

                {0, 0},

                {0, size-1},

                {size-1, 0},

                {size-1, size-1}

            };



            // Alle Ecken durchprobiert - Hunt-Phase beendet

            if (shotIndex >= 4){

                return false;

            }



            int tx = corners[shotIndex][0];

            int ty = corners[shotIndex][1];

            shotIndex++;



            // Nur auf ungetätigte Schüsse zielen (field[tx][ty] == 0)

            if(field[tx][ty] == 0){

                x = tx;

                y = ty;

                return true;

            }

            continue;

        }



        // Wenn alle Felder einer Phase durchsucht: zur nächsten Phase übergehen

        if (shotIndex >= size*size){

            shotIndex = 0;

            phase++;

            continue;

        }



        // Berechne Koordinaten aus Index: z.B. Index 5 in 10x10 Feld = (0,5)

        int tx = shotIndex / size;

        int ty = shotIndex % size;

        shotIndex++;



        // Überspringe Ecken (werden in Phase 2 behandelt)

        if (is_corner(tx, ty, size)){

            continue;

        }



        // Phase 0: Nur Felder schießen, wo die Summe ungerade ist

        if (phase == 0 && (tx + ty) % 2 != 0){

            continue;

        }



        // Phase 1: Nur Felder schießen, wo die Summe gerade ist

        if (phase == 1 && (tx + ty) % 2 != 1){

            continue;

        }



        // Gültiger Schuß gefunden - auf noch nicht getätigen Feld

        if(field[tx][ty] == 0){

            x = tx;

            y = ty;

            return true;

        }

    }
}



/**

 * DESTROY PHASE - Zerstört ein bereits getroffenes Schiff systematisch

 *

 * Sobald ein Schiff getroffen wird (SHIP_HIT), wechselt diese Funktion in den

 * Zerstörungsmodus. Sie versucht, das gesamte Schiff zu zerstören durch:

 *  1. Überprüfung der vier Hauptdirektionen (oben, unten, links, rechts)

 *  2. Systematische Vergrößerung der Entfernung in jede Richtung

 *

 * @param size Größe des Spielfelds

 * @param field Array für getätigte Schüsse

 * @param x Ausgabe: X-Koordinate des nächsten Schusses

 * @param y Ausgabe: Y-Koordinate des nächsten Schusses

 * @param last_x X-Koordinate des letzten Treffers

 * @param last_y Y-Koordinate des letzten Treffers

 * @param reset Wenn true, wird die Destroy-Phase zurückgesetzt

 * @return true wenn ein Schuß möglich ist, false wenn Schiff vollständig zerstört

 */

bool nextDestroyShot(int size, int field[][10], int &x, int &y, int last_x, int last_y, bool reset = false){



    // Statische Variablen für Zustandsverwaltung über mehrere Aufrufe

    static int destroyPhase = 0;  // Aktuelle Richtung (0=oben, 1=unten, 2=links, 3=rechts)

    static int distance = 1;      // Wie weit in die aktuelle Richtung gegangen wird



    // Zurücksetzen der Destroy-Phase

    if(reset){

        destroyPhase = 0;

        distance = 1;

        return false;

    }



    // Direktions-Vektoren: oben, unten, links, rechts

    int dx[] = {-1,  1,  0, 0};

    int dy[] = { 0,  0, -1, 1};



    // Schleife zum Durchsuchen aller Richtungen und Entfernungen

    while(true){

        // Alle 4 Richtungen durchprobiert - Entfernung erhöhen

        if(destroyPhase >= 4){

            distance++;



            // Maximale Entfernung überschritten - Schiff vollständig zerstört

            if(distance > size){

                return false;

            }



            // Zurück zur ersten Richtung mit erhöhter Entfernung

            destroyPhase = 0;

        }



        // Berechne Zielkoordinaten basierend auf letztem Treffer, Richtung und Entfernung

        int tx = last_x + dx[destroyPhase] * distance;

        int ty = last_y + dy[destroyPhase] * distance;

        destroyPhase++;  // Zur nächsten Richtung übergehen



        // Überprüfe ob Zielfeld im Spielfeld liegt

        if(tx < 0 || tx >= size || ty < 0 || ty >= size){

            continue;

        }



        // Gültiger Schuß gefunden - auf noch nicht getätigen Feld

        if(field[tx][ty] == 0){

            x = tx;

            y = ty;

            return true;

        }

    }

}




/**

 * HAUPTPROGRAMM - Spielclient für Battleship

 *

 * Der Client verbindet sich mit einem Server und spielt mehrere Spiele Battleship.

 * Strategie:

 *  1. Hunt-Phase: Systematische Suche nach Schiffen mit Schachbrettmuster

 *  2. Destroy-Phase: Zerstört getroffene Schiffe durch gezielte Schüsse in vier Richtungen

 *  3. Wiederholt das Spiel bis zum Ende aller Runden

 */

int main() {

    // Zufallsgenerator mit aktuellem Zeitstempel initialisieren

    srand(time(NULL));



    // TCP-Client für Verbindung zum Server

    TCPclient c;

    string host = "127.0.0.1";



    // Verbinde zu Host auf Port 2022

    c.conn(host, 2022);



    // Kontroll-Variable für Spielschleife

    bool goOn = 1;



    // Spielverwaltung

    int nmb_of_games = 100;  // Anzahl der zu spielenden Spiele

    int count_games = 0;      // Zähler für gespielte Spiele



    // Kommunikation mit Server

    string msg_recieved;

    string msg;

    char response[16] = {0};  // Gepufferte Antwort vom Server



    // Spielfeld (10x10)

    const int size = 10;

    int field[size][10] = {0};  // 0 = nicht geschossen, 1 = geschossen



    int count_shots = 0;  // Schußzähler für aktuelle Spiel

    int x, y;             // Koordinaten des nächsten Schusses



    // Hunt/Destroy Phase Management

    bool inDestroyMode = false;  // Bin ich gerade im Zerstörungsmodus?

    int last_x = -1;             // Koordinaten des letzten Treffers

    int last_y = -1;



    // Neues Spiel anfordern

    msg = "NEWGAME\n";

    c.sendData(msg);

    sleep(1);

    msg_recieved = c.receive(200);  // Warte auf Bestätigung

    sleep(1);



    // Hauptspielschleife

    while (goOn){



        // Extrahiere erste Wort der Antwort in response-Buffer

        if (sscanf(msg_recieved.c_str(), "%15s", response) != 1){

            continue;

        }



        // SPIEL ZUENDE - Verarbeite GAME_OVER Nachricht

        if (strcmp(response, "GAME_OVER") == 0){

            count_games++;



            // Habe ich alle Spiele abgeschlossen?

            if(nmb_of_games <= count_games){

                // Ausgabe der Schüsse und Beende Verbindung

                cout << to_string(count_shots) << endl;

                msg = string("BYEBYE");

                c.sendData(msg);

                goOn = 0;

                break;

            }

            else {

                // Starte neues Spiel

                msg = string("NEWGAME\n");

                cout << to_string(count_shots) << endl;

                count_shots = 0;



                // Setze Spielfeld zurück

                for (int i = 0; i < size; i++){

                    for(int j = 0; j < size; j++){

                        field[i][j] = 0;

                    }

                }



                // Stelle Hunt- und Destroy-Phase zurück

                inDestroyMode = false;

                last_x = -1;

                last_y = -1;

                nextHuntShot(size, field, x, y, true);        // Setze Hunt-Phase zurück

                nextDestroyShot(size, field, x, y, last_x, last_y, true);  // Setze Destroy-Phase zurück



                c.sendData(msg);

                sleep(0);

                msg_recieved = c.receive(200);  // Warte auf Bestätigung des neuen Spiels

                sleep(0);

                continue;

            }

        }



        // SPIELPHASE - Schießt noch Schüsse?

        else if (count_shots < size * size){



            bool haveTarget = false;



            // Bestimme nächsten Schuß basierend auf aktuellem Modus

            if(!inDestroyMode){

                // Hunt-Phase: Suche nach Schiffen

                haveTarget = nextHuntShot(size, field, x, y);

            }

            else {

                // Destroy-Phase: Zerstöre getroffenes Schiff

                haveTarget = nextDestroyShot(size, field, x, y, last_x, last_y);



                // Wenn kein Schuß mehr in Destroy-Phase möglich, zurück zur Hunt-Phase

                if (!haveTarget){

                    inDestroyMode = false;

                    haveTarget = nextHuntShot(size, field, x, y);

                }

            }



            // Wenn kein gültiger Schuß gefunden, warte auf nächste Antwort

            if(!haveTarget){

                continue;

            }



            // Sende Schuß an Server (Koordinaten sind 1-basiert)

            msg = "SHOT " + to_string(x+1) + " " + to_string(y+1) + "\n";

            count_shots++;

            field[x][y] = 1;  // Markiere Feld als geschossen



            c.sendData(msg);

            sleep(0);

            msg_recieved = c.receive(200);  // Warte auf Antwort (SHIP_HIT, SHIP_DESTROYED, SHIP_MISSED)

            sleep(0);



            // Verarbeite Antwort vom Server

            sscanf(msg_recieved.c_str(), "%15s", response);



            // Treffer erzielt - starte Destroy-Phase

            if(strcmp(response, "SHIP_HIT") == 0){

                if(!inDestroyMode){

                    inDestroyMode = true;

                    last_x = x;

                    last_y = y;

                    nextDestroyShot(size, field, x, y, last_x, last_y, true);  // Initialisiere Destroy-Phase

                }

            }

            // Schiff vollständig zerstört - zurück zur Hunt-Phase

            else if (strcmp(response, "SHIP_DESTROYED") == 0){

                inDestroyMode = false;

                last_x = -1;

                last_y = -1;

                nextHuntShot(size, field, x, y, true);  // Setze Hunt-Phase zurück

            }

        }



        // FEHLERBEHANDLUNG

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



