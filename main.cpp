//=====[Libraries]=============================================================

#include "VirtualFence.h"

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    virtualFence farm;
    while(true) {
        farm.update();
    }
}