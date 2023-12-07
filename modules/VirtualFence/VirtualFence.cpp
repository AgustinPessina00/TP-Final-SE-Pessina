//=====[Libraries]=============================================================

#include "VirtualFence.h"
#include "Stimulus.h"

//=====[Declaration of private defines]========================================

#define NUMBER_OF_FENCES    3
#define FENCE_DURATION      6000000
#define FENCE_RADIO         300
#define LATENCY             2000

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//CERCO PARA CAPITAL.
//static float fenceLatitudes[NUMBER_OF_FENCES] = {-34.602729, -34.603278};
//static float fenceLongitudes[NUMBER_OF_FENCES] = {-58.422540, -58.418032};

//CERCO PARA LOBOS.
static float fenceLatitudes[NUMBER_OF_FENCES] = {-35.192304, -35.192534, -35.192270};
static float fenceLongitudes[NUMBER_OF_FENCES] = {-59.099840, -59.096181, -59.095471};

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
    static float flat;
    static float flon;
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
        this->trackerGPS1->positionUpdate( str, &flat, &flon);
        this->stimulusUpdate(flat, flon);

        this->gsmGprs->transmitionStart();

        while(this->trackerGPS1->sendCoordAnimal == true) {
            this->gsmGprs->connect();
            this->gsmGprs->send(str);

            if (this->gsmGprs->transmitionHasEnded ()) {
                this->gsmGprs->disconnect ();
                this->gsmGprs->transmitionStop();
                if (this->gsmGprs->disconnectionProcessHasEnded()) {
                    this->latency->write( LATENCY );
                    this->trackerGPS1->sendCoordAnimal = false;
                    //printf("\r\n MANDO POR ACÁ EL CERCO JAJAJA \r\n");
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
        this->fenceChange->write( FENCE_DURATION );
    }
    else {
        this->fenceToSend = false;
    }
}

void virtualFence::stimulusUpdate(float flat, float flon) {
    float distance = distanceBetween(fenceLatitude, fenceLongitude, flat, flon);
    printf("\r\nLA DISTANCIA ENTRE AMBOS PUNTOS ES %f \r\n", distance);

    if( distance <= FENCE_RADIO ) {
        setDutyCycle( 0.0f );
    }
    else if( distance <= (FENCE_RADIO + 10) ) {
        setDutyCycle( 0.1f );
    }
    else if( distance <= (FENCE_RADIO + 20) ) {
        setDutyCycle( 0.2f );
    }
    else if( distance <= (FENCE_RADIO + 30) ) {
        setDutyCycle( 0.3f );
    }
    else if( distance <= (FENCE_RADIO + 40) ) {
        setDutyCycle( 0.4f );
    }
    else if( distance <= (FENCE_RADIO + 50) ) {
        setDutyCycle( 0.5f );
    }
    else if( distance <= (FENCE_RADIO + 60) ) {
        setDutyCycle( 0.6f );
    }
    else if( distance <= (FENCE_RADIO + 70) ) {
        setDutyCycle( 0.7f );
    }
    else if( distance <= (FENCE_RADIO + 80) ) {
        setDutyCycle( 0.8f );
    }
    else if( distance <= (FENCE_RADIO + 90) ) {
        setDutyCycle( 0.9f );
    }
    else {
        setDutyCycle( 1.f );
    }

}


//=====[Implementations of private methods]====================================

//=====[Implementations of public functions]===================================

//=====[Implementations of private functions]==================================
