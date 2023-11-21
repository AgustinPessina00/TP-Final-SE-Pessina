//=====[#include guards - begin]===============================================

#ifndef _VirtualFence_H_
#define _VirtualFence_H_

#include "nonBlockingDelay.h"
#include "TrackerGPS.h"
#include "GsmGprsCom.h"

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================================

class virtualFence {
//=====[Declaration of public methods]=========================================
public:
    virtualFence();
    void update();
    void fenceUpdate();
    void fenceSend();
    bool getStateFence();
    float getFenceLatitude();
    float getFenceLongitude();
private:
//=====[Declaration of privates atributes]=====================================
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