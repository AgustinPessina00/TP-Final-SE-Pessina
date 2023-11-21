//=====[Libraries]=============================================================

#include "TrackerGPS.h"

//=====[Declaration of private defines]========================================

#define LATENCY     20000

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut LED( LED2 );
DigitalOut LED_2( LED3 );
UnbufferedSerial uartComUSB( USBTX, USBRX, 115200 ); // debug only
BufferedSerial uartGPSCom( PG_14, PG_9, 9600 );

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]====================================

//=====[Implementations of public functions]===================================

/** 
* @brief Contructor method creates a new trackerGPS instance ready to be used
*/
trackerGPS::trackerGPS ()
{
    this->gsmGprs = new gsmGprsCom( );
    this->numberOfDevice = 1;
    LED = ON;
    LED_2 = ON;
    TinyGPS();
}

/** 
* @brief Main rutine of the tracker device
* When the time set on the delay pass
* the tracker will recive new geolocalization from the GPS module, and it will parse that data into a format "ID|LAT|LON\r\n"
* then the tracker will connect his GSM GPRS module to the GSM GPRS network, when it succeded the module will connect to a TCP Server
* after that the data will be sent, the TCP it's dropped, and a new delay is set and the process will be repeated.
*/
void trackerGPS::positionUpdate ( char * str)
{
    char c  = '\0';
    static bool readyToReadNewGeo = true;
//  static bool timeToSend = false;
    float flat, flon;
    unsigned long age;
/*
    if( this->latency->read() || (timeToSend == true) ) {
        timeToSend = true;
*/
    while ( uartGPSCom.readable() ) {
        uartGPSCom.read(&c, 1); 
        if (encode(c) && (readyToReadNewGeo == true)) {
            f_get_position(&flat, &flon, &age);
            sprintf ( str, "%d|%.7f|%.7f|False\r\n", this->numberOfDevice , flat, flon);
            uartComUSB.write( str, strlen(str) );
            readyToReadNewGeo = false;
        }
    }
    readyToReadNewGeo = true;
//  timeToSend = false;
    } 
}

//=====[Implementations of private functions]==================================
