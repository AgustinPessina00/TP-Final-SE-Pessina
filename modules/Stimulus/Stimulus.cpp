//=====[Libraries]=============================================================

#include "arm_book_lib.h"
#include "Stimulus.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

PwmOut stimulus = PD_12;

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static void setPeriod( float period );

//=====[Implementations of public functions]===================================

void stimulusControlInit() {
    setPeriod( 0.01f );

    setDutyCycle( 0.f );
}

void setDutyCycle( float dutyCycle ) {
    stimulus.write(dutyCycle);
}

//=====[Implementations of private functions]==================================

static void setPeriod( float period ) {
    stimulus.period(period);
}