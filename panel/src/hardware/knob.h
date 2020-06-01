#ifndef _KNOB_H
#define _KNOB_H

#include <Arduino.h>
#include "../../lib/functional-avr/nonstd.h"

const int kMaxKnobUpdateCallbackCount = 1;

enum KnobUpdateEvent {
    KnobUpdateEventUnknown,
    Clockwise,
    CounterClockwise,
    Click
};
typedef nonstd::function<void(KnobUpdateEvent, int)> KnobUpdateCallback;

// The Knob class maintains the states of a Knob (backed by a rotary encoder) and
// checks if the knob has been turned when Update() is called.
class Knob {
    public:
        Knob(const char* knobName, int pinClk, int pinDt, int pulseDegree);
        // Update checks if the knob has been turned since the last check.
        bool Update();
        
        void RegisterUpdateCallback(KnobUpdateCallback callback);
    protected:
        const char* knobName_;
        int pinClk_;
        int pinDt_;
        int pulseDegree_;  // Number of degrees turned for one pulse
        int lastClk_;
        int degree_;
        unsigned int loopsSinceLastUpdate_ = 0;

        int updateCallbackCount_ = 0;
        KnobUpdateCallback updateCallbacks_[kMaxKnobUpdateCallbackCount];
};

#endif  // _KNOB_H
