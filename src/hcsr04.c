#include "hcsr04.h"

static struct {
    unsigned short *distance;   //Variable (address) specified by user, for distance readings

} HCS;


void HCinit(unsigned short *distanceAdd) {
    HCS.distance=distanceAdd;
}

bool HCread () {
    
    return false;
}
