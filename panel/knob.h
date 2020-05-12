#ifndef _KNOB_H
#define _KNOB_H

#include <Arduino.h>

// The Knob class maintains the states of a Knob (backed by a rotary encoder) and
// checks if the knob has been turned when Update() is called.
class Knob {
    public:
        Knob(const char* knobName, int pinClk, int pinDt, int pulseDegree);
        // Update checks if the knob has been turned since the last check.
        bool Update();
    private:
        const char* knobName;
        int pinClk;
        int pinDt;
        int pulseDegree;  // Number of degrees turned for one pulse
        int lastClk;
        int degree;
        unsigned int loopsSinceLastUpdate = 0;
};

#endif  // _KNOB_H
