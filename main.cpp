//=====[Libraries]=============================================================

#include "VirtualFence.h"
#include "Stimulus.h"
#include <string.h>
#include "TinyGPS.h"

//=====[Main function, the program entry point after power on or reset]========

int main() {
    virtualFence farm;
    stimulusControlInit();
    while(true) {
        farm.update();
    }
}