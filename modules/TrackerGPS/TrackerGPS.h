//=====[#include guards - begin]===============================================
#ifndef _TrackerGPS_H_
#define _TrackerGPS_H_

//======[Libraries]=============================================================
#include "GsmGprsCom.h"
#include "TinyGPS.h"
#include "nonBlockingDelay.h"
#include "arm_book_lib.h"

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================================

/*
 * Class implementation for a GPS tracker
 */
class trackerGPS {
public:
    trackerGPS ();
    void positionUpdate( char * str, float *flat, float *flon);
    bool sendCoordAnimal;
private:
    gsmGprsCom * gsmGprs;
    int numberOfDevice;
};

//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _TrackerGPS_H_