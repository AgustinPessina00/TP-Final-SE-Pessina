//=====[Libraries]=============================================================

#include "VirtualFence.h"
#include "Stimulus.h"
#include <string.h>
#include "TinyGPS.h"

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

/* PRUEBA DISTANCIA ENTRE COORDENADAS:
#include <cmath>

#define earthRadiusKm 6371.0
#define metersInKm 1000.0

float deg2rad(float deg) {
    return (deg * M_PI / 180);
}

float distance(float lat1, float lon1, float lat2, float lon2) {
    float dLat = deg2rad(lat2 - lat1);
    float dLon = deg2rad(lon2 - lon1);

    float a = sin(dLat / 2) * sin(dLat / 2) +
               cos(deg2rad(lat1)) * cos(deg2rad(lat2)) *
               sin(dLon / 2) * sin(dLon / 2);

    float c = 2 * atan2(sqrt(a), sqrt(1 - a));
    float distance = earthRadiusKm * c * metersInKm; // Distance in meters

    return distance;
}

int main() {
    float lat1 = -35.191826;
    float lon1 = -59.100395;
    float lat2 = -35.191815;
    float lon2 = -59.097187;

    while(true) {
        printf("LA DISTANCIA ENTRE LOS PUNTOS ES: %f\r\n", distance(lat1, lon1, lat2, lon2));
        delay(5000);
    }

}
*/