#include "knob.h"

#include "../../debug.h"

const int kMinLoopsBetweenUpdates = 5;

Knob::Knob(const char* knobName, int pinClk, int pinDt, int pulseDegree) {
    this->knobName_ = knobName;
    this->pinClk_ = pinClk;
    this->pinDt_ = pinDt;
    this->pulseDegree_ = pulseDegree;

    pinMode(this->pinClk_, INPUT_PULLUP);
    pinMode(this->pinDt_, INPUT_PULLUP);
    this->lastClk_ = digitalRead(this->pinClk_);
    this->degree_ = 0;

    CONSOLE_PRINT("Initialized knob ");
    CONSOLE_PRINT(this->knobName_);
    CONSOLE_PRINT(" CLK=");
    CONSOLE_PRINT(this->pinClk_);
    CONSOLE_PRINT(" DT=");
    CONSOLE_PRINTLN(this->pinDt_);
}

bool Knob::Update() {
    loopsSinceLastUpdate_++;
    int valClk = digitalRead(pinClk_);
    int valDt = digitalRead(pinDt_);
    if (valClk != lastClk_) {
        DEBUG_PRINT("Reading pin ");
        DEBUG_PRINT(pinClk_);
        DEBUG_PRINT(" for knob ");
        DEBUG_PRINT(knobName_);
        DEBUG_PRINT(" last ");
        DEBUG_PRINT(lastClk_);
        DEBUG_PRINT(" now ");
        DEBUG_PRINTLN(valClk);

        lastClk_ = valClk;
        if (loopsSinceLastUpdate_ < kMinLoopsBetweenUpdates) {
            // With kCheckIntervalUsNonUpdateState = 5000 and kCheckIntervalUsUpdateState = 100,
            // we check the encoder pins between 200 and 10k Hz. If two updates were observed too close
            // to each other, there has likely been a bounce, or a loop may be delayed by serial
            // operation or interrupts. In both cases, we want to ignore the update.
            DEBUG_PRINT("Knob ");
            DEBUG_PRINT(knobName_);
            DEBUG_PRINT(" state changed only after ");
            DEBUG_PRINT(loopsSinceLastUpdate_);
            DEBUG_PRINTLN(" loops, skipped");
            return false;
        }

        KnobUpdateEvent event;
        DEBUG_PRINT("Knob ");
        DEBUG_PRINT(knobName_);
        if (valClk != valDt) {
            // clockwise
            event = Clockwise;
            DEBUG_PRINT(" turned clockwise to ");
            degree_ = (degree_ + pulseDegree_) % 360;
        } else {
            // counter-clockwise
            event = CounterClockwise;
            DEBUG_PRINT(" turned counter-clockwise to ");
            degree_ = (degree_ - pulseDegree_ + 360) % 360;
        }
        DEBUG_PRINTLN(degree_);        
        for (int i = 0; i < updateCallbackCount_; i++) {
            updateCallbacks_[i](event, degree_);
        }
        loopsSinceLastUpdate_ = 0;
        return true;
    }
    return false;
}

void Knob::RegisterUpdateCallback(KnobUpdateCallback callback) {
    if (updateCallbackCount_ >= kMaxKnobUpdateCallbackCount) {
        CONSOLE_PRINT("Knob ");
        CONSOLE_PRINT(knobName_);
        CONSOLE_PRINT(" already has ");
        CONSOLE_PRINT(updateCallbackCount_);
        CONSOLE_PRINTLN(" update callbacks. Cannot register more.");
        return;
    }
    updateCallbacks_[updateCallbackCount_] = callback;
    updateCallbackCount_++;
}