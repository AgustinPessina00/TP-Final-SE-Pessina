//=====[Libraries]=============================================================

#include "VirtualFence.h"

//=====[Declaration of private defines]========================================

#define NUMBER_OF_FENCES    4
#define FENCE_DURATION      50000
#define FENCE_RADIO         300
#define LATENCY             20000

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static float fenceLatitudes[NUMBER_OF_FENCES] = {1, 2, 3 , 4};
static float fenceLongitudes[NUMBER_OF_FENCES] = {1, 2, 3, 4};

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]=====================================

virtualFence::virtualFence() {
    this->fenceToSend = false;
    this->fenceNumber = 0;
    this->fenceLatitude = fenceLatitudes[this->fenceNumber];
    this->fenceLongitude = fenceLongitudes[this->fenceNumber];
    this->fenceRadio = FENCE_RADIO;
    this->fenceChange = new nonBlockingDelay( FENCE_DURATION );

    this->trackerGPS1 = new trackerGPS();

    this->latency = new nonBlockingDelay( LATENCY );
    this->gsmGprs = new gsmGprsCom( );
    this->gsmGprs->transmitionStop( );
}

void virtualFence::update() {
    static char str[100] = "";
//TRANSMITO LA UBICACIÓN DEL CERCO VIRTUAL
    this->fenceUpdate( );
    if( this->fenceToSend ) {
        sprintf ( str, "0|%.7f|%.7f|True\r\n", this->fenceLatitude, this->fenceLongitude);

        this->gsmGprs->transmitionStart();
        this->gsmGprs->connect();
        this->gsmGprs->send(str);
        if (this->gsmGprs->transmitionHasEnded ()) {
            this->gsmGprs->disconnect ();
            this->gsmGprs->transmitionStop();
            if (this->gsmGprs->disconnectionProcessHasEnded()) {
                this->latency->write( FENCE_DURATION );
            }
        }
    }

//TRANSMITO LA UBICACIÓN DEL ANIMAL.
    if(this->latency->read()) {
        this->trackerGPS1->positionUpdate( str ); 

        this->gsmGprs->transmitionStart();
        this->gsmGprs->connect();
        this->gsmGprs->send(str);

        if (this->gsmGprs->transmitionHasEnded ()) {
            this->gsmGprs->disconnect ();
            this->gsmGprs->transmitionStop();
            if (this->gsmGprs->disconnectionProcessHasEnded()) {
                this->latency->write( LATENCY );
            }
        }
    }
}

void virtualFence::fenceUpdate() {
    if( this->fenceChange->read() ) {
        this->fenceNumber++;
        if( this->fenceNumber >= NUMBER_OF_FENCES ) {
            this->fenceNumber = 0;
        }
        this->fenceLatitude = fenceLatitudes[this->fenceNumber];
        this->fenceLongitude = fenceLongitudes[this->fenceNumber];
        this->fenceToSend = true;
    }
    else {
        this->fenceToSend = false;
    }
}

void virtualFence::fenceSend() {

}

//=====[Implementations of private methods]====================================

//=====[Implementations of public functions]===================================

//=====[Implementations of private functions]==================================
