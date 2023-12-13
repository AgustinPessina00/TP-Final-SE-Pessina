//=====[#include guards - begin]===============================================

#ifndef _VirtualFence_H_
#define _VirtualFence_H_

#include "nonBlockingDelay.h"
#include "TrackerGPS.h"
#include "GsmGprsCom.h"

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================================

/*
 * Class implementation for virtualFence
 * it will be instantiated and used from the main function
 * handles a GPS module, a GSM GPRS module and a delay in order to send the geolocation of the device
 * to a TCP IP server
 */
class virtualFence {
public:
    //Public methods
    virtualFence();
    void update();
private:
    //Private methods
    void fenceUpdate();
    void stimulusUpdate(float flat, float flon);
    //Private atributes
    bool fenceToSend;
    float fenceLatitude;
    float fenceLongitude;
    int fenceRadio;
    int fenceNumber;
    nonBlockingDelay * fenceChange;
    trackerGPS * trackerGPS1;
    gsmGprsCom * gsmGprs;
    nonBlockingDelay * latency;
};

//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _VirtualFence_H_