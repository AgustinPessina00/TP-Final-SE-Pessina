//=====[Libraries]=============================================================

#include "VirtualFence.h"
#include "Stimulus.h"
#include <string.h>

//=====[Main function, the program entry point after power on or reset]========

int main() {
    virtualFence farm;
    stimulusControlInit();
    while(true) {
        farm.update();
    }
}

/*PUREBA GPRS
gsmGprsCom gsmGprs;
    bool termina = true;
    char str[] = "1|-34.602729|-58.422540|False";
    gsmGprs.transmitionStart();
    while(termina) {
        gsmGprs.connect();
        gsmGprs.send(str);
            if (gsmGprs.transmitionHasEnded ()) {
                gsmGprs.disconnect ();
                gsmGprs.transmitionStop();
                if (gsmGprs.disconnectionProcessHasEnded())
                    printf("HOLA\n");
                    bool termina = false;
            }
    }
    return 0;  
*/

    /* PRUEBA GPS
    nonBlockingDelay* latency = new nonBlockingDelay ( LATENCY );
    TinyGPS();
    char str[100];
    int numberOfDevice = 1;
    char c  = '\0';
    static bool readyToReadNewGeo = true;
    static bool timeToSend = false;
    float flat, flon;
    unsigned long age;

    while(true) {
        if( latency->read() || (timeToSend == true) ) {
            timeToSend = true;

        while ( uartGPSCom.readable() ) {
            uartGPSCom.read(&c, 1); 
            if (encode(c) && (readyToReadNewGeo == true)) {
                f_get_position(&flat, &flon, &age);
                sprintf ( str, "%d|%.7f|%.7f|False\r\n", numberOfDevice , flat, flon);
                uartComUSB.write( str, strlen(str) );
                uartComUSB.write( "HOLA\r\n", 4);
                readyToReadNewGeo = false;
            }
        }
        readyToReadNewGeo = true;
        timeToSend = false;
        //latency->write(5000);
        }
    }
    */