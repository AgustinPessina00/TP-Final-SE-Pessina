//=====[Libraries]=============================================================

#include "VirtualFence.h"
#include "Stimulus.h"

//=====[Declaration of private defines]========================================

#define NUMBER_OF_FENCES    2
#define FENCE_DURATION      6000000
#define FENCE_RADIO         300
#define LATENCY             2000

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static float fenceLatitudes[NUMBER_OF_FENCES] = {-34.602729, -34.603278};
static float fenceLongitudes[NUMBER_OF_FENCES] = {-58.422540, -58.418032};

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]=====================================

virtualFence::virtualFence() {
    this->fenceToSend = true;
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
    bool sendCoordFence = true;
    bool sendCoordAnimal = true;
//TRANSMITO LA UBICACIÓN DEL CERCO VIRTUAL
    if( this->fenceToSend ) {
        sprintf ( str, "0|%.7f|%.7f|True\r\n", this->fenceLatitude, this->fenceLongitude);

        this->gsmGprs->transmitionStart();
        
        while(sendCoordFence == true) {
            this->gsmGprs->connect();
            this->gsmGprs->send(str);
            if (this->gsmGprs->transmitionHasEnded ()) {
                this->gsmGprs->disconnect ();
                this->gsmGprs->transmitionStop();
                if (this->gsmGprs->disconnectionProcessHasEnded()) {
                    sendCoordFence = false;
                    this->fenceToSend = false;
                }
            }
        }
    }
    this->fenceUpdate( );

//TRANSMITO LA UBICACIÓN DEL ANIMAL.
    if(this->latency->read()) {
        this->trackerGPS1->positionUpdate( str );

        this->gsmGprs->transmitionStart();

        while(sendCoordAnimal == true) {
            this->gsmGprs->connect();
            this->gsmGprs->send(str);

            if (this->gsmGprs->transmitionHasEnded ()) {
                this->gsmGprs->disconnect ();
                this->gsmGprs->transmitionStop();
                if (this->gsmGprs->disconnectionProcessHasEnded()) {
                    this->latency->write( LATENCY );
                    sendCoordAnimal = false;
                }
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
        this->latency->write( FENCE_DURATION );
    }
    else {
        this->fenceToSend = false;
    }
}

void virtualFence::stimulusUpdate() {
    if(distance_between (fenceLatitude, fenceLongitude, , float long2) )
}

//=====[Implementations of private methods]====================================

//=====[Implementations of public functions]===================================

//=====[Implementations of private functions]==================================
