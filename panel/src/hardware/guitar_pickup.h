#ifndef _GUITAR_PICK_H
#define _GUITAR_PICK_H

#include <Arduino.h>
#include "../../lib/functional-avr/nonstd.h"

const int kMaxGuitarPickupUpdateCallbackCount = 1;

typedef nonstd::function<void(int)> GuitarPickupUpdateCallback;

// The GuitarPickup class maintains the states of a mutli-position selector
// (backed by a guitar pickup selector) and checks if the selector has been moved
// when Update() is called.
// The pickup selector is based on https://www.amazon.co.uk/gp/product/B0092MDKG8/
// I could not find a manual explaining the wiring inside the selector, and here's
// the reserve-engineering result:
// Facing the side of the selector with the word "ALPHA" to you, with the lever
// pointing to the left. From top to bottom, let's call the lever's position 0, 1, 2, 3, 4.
// From top to bottom, let's call the pins 0, 1, 2, 3, 4, 5, 6, 7 .
// The following pin groups are connected when the lever is put in each position:
// Lever position 0: pin 3-2   pin 4-7
// Lever position 1: pin 3-1-2 pin 4-6-7
// Lever position 2: pin 3-1   pin 4-6
// Lever position 3: pin 3-0-1 pin 4-5-6
// Lever position 4: pin 3-0   pin 4-5
// Pins between 0 and 3 are never connected to pins between 4 and 7.
// In the following code, it is assumed that pin 3 is connected to GND and
// pin 0, 1 and 2 will be used as input with a pullup.

class GuitarPickup {
    public:
        GuitarPickup(const char* guitarPickupName, int pins[3]);
        // Update checks if the selector has been moved since the last check.
        bool Update();
        
        void RegisterUpdateCallback(GuitarPickupUpdateCallback callback);
    protected:
        const char* guitarPickupName_;
        int pins_[3];
        int lastGrayCode_;

        int updateCallbackCount_ = 0;
        GuitarPickupUpdateCallback updateCallbacks_[kMaxGuitarPickupUpdateCallbackCount];
};

#endif  // _GUITAR_PICK_H
