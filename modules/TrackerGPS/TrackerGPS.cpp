//=====[Libraries]=============================================================

#include "TrackerGPS.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

UnbufferedSerial uartComUSB( USBTX, USBRX, 115200 );    //FOR DEBUG
BufferedSerial uartGPSCom( PG_14, PG_9, 9600 );

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]====================================

//=====[Implementations of public functions]===================================

//-----------------------------------------------------------------------------
trackerGPS::trackerGPS () {
    this->gsmGprs = new gsmGprsCom( );
    this->numberOfDevice = 1;
    this->sendCoordAnimal = false;
    TinyGPS();
}

//-----------------------------------------------------------------------------
void trackerGPS::positionUpdate ( char * str, float *flat, float *flon) {
    char c  = '\0';
    static bool readyToReadNewGeo = true;
    static bool timeToSend = false;
    unsigned long age;

    while( uartGPSCom.readable( ) ) {
        uartGPSCom.read(&c, 1); 
        if( encode(c) && ( readyToReadNewGeo == true ) ) {
            f_get_position( flat, flon, &age );
            sprintf ( str, "%d|%.7f|%.7f|False\r\n", this->numberOfDevice , *flat, *flon );
            uartComUSB.write( str, strlen(str) );
            readyToReadNewGeo = false;
            this->sendCoordAnimal = true;
        }
    }
    readyToReadNewGeo = true;
} 

//=====[Implementations of private functions]==================================
