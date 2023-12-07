//=====[#include guards - begin]===============================================

#ifndef _TrackerGPS_H_
#define _TrackerGPS_H_

#include "GsmGprsCom.h"
//#include "VirtualFence.h"
#include "TinyGPS.h"
#include "nonBlockingDelay.h"
#include "arm_book_lib.h"

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================================

/*
 * Class implementation for a GPS tracker
 * High hierarchy class
 * it will be instantiated and used from the main function
 * handles a GPS module, a GSM GPRS module and a delay in order to send the geolocation of the device
 * to a TCP IP server
 */
class trackerGPS {
public:
    trackerGPS ();
    void positionUpdate( char * str, float *flat, float *flon);
    bool sendCoordAnimal;
private:
    gsmGprsCom * gsmGprs;
    nonBlockingDelay * latency;
    int numberOfDevice;
};

//=====[Declarations (prototypes) of public functions]=========================


//=====[#include guards - end]=================================================

#endif // _TrackerGPS_H_