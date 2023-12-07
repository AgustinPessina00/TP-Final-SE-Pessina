//=====[#include guards - begin]===============================================

#ifndef _nonBlockingDelay_H_
#define _nonBlockingDelay_H_

//=====[Libraries]=============================================================

#include "mbed.h"

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

typedef uint64_t tick_t;

//=====[Declaration of public classes]=========================================

class nonBlockingDelay {
    public:
//Public methods
    nonBlockingDelay( tick_t durationValue );
    void write( tick_t durationValue );
    bool read( );

    private:
//Private atributtes
    tick_t startTime;
    tick_t duration;
    bool isRunning;
    Ticker ticker;
    
//Private methods
    void tickInit( );
};

//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _nonBlockingDelay_H_