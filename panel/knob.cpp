#include "knob.h"

#include "debug.h"

#define MIN_LOOPS_BETWEEN_UPDATES 5

Knob::Knob(const char* knobName, int pinClk, int pinDt, int pulseDegree) {
    this->knobName = knobName;
    this->pinClk = pinClk;
    this->pinDt = pinDt;
    this->pulseDegree = pulseDegree;

    pinMode(this->pinClk, INPUT_PULLUP);
    pinMode(this->pinDt, INPUT_PULLUP);
    this->lastClk = digitalRead(this->pinClk);
    this->degree = 0;

    CONSOLE_PRINT("Initialized knob ");
    CONSOLE_PRINT(this->knobName);
    CONSOLE_PRINT(" CLK=");
    CONSOLE_PRINT(this->pinClk);
    CONSOLE_PRINT(" DT=");
    CONSOLE_PRINTLN(this->pinDt);
}

bool Knob::Update() {
    loopsSinceLastUpdate++;
    int valClk = digitalRead(pinClk);
    int valDt = digitalRead(pinDt);
    if (valClk != lastClk) {
        DEBUG_PRINT("Reading pin ");
        DEBUG_PRINT(pinClk);
        DEBUG_PRINT(" for knob ");
        DEBUG_PRINT(knobName);
        DEBUG_PRINT(" last ");
        DEBUG_PRINT(lastClk);
        DEBUG_PRINT(" now ");
        DEBUG_PRINTLN(valClk);

        lastClk = valClk;
        if (loopsSinceLastUpdate < MIN_LOOPS_BETWEEN_UPDATES) {
            // With CHECK_INTERVAL_US_NON_UPDATE_STATE = 5000 and CHECK_INTERVAL_US_UPDATE_STATE = 100,
            // we check the encoder pins between 200 and 10k Hz. If two updates were observed too close
            // to each other, there has likely been a bounce, or a loop may be delayed by serial
            // operation or interrupts. In both cases, we want to ignore the update.
            DEBUG_PRINT("Knob ");
            DEBUG_PRINT(knobName);
            DEBUG_PRINT(" state changed only after ");
            DEBUG_PRINT(loopsSinceLastUpdate);
            DEBUG_PRINTLN(" loops, skipped");
            return false;
        }

        KnobUpdateEvent event;
        DEBUG_PRINT("Knob ");
        DEBUG_PRINT(knobName);
        if (valClk != valDt) {
            // clockwise
            event = Clockwise;
            DEBUG_PRINT(" turned clockwise to ");
            degree = (degree + pulseDegree) % 360;
        } else {
            // counter-clockwise
            event = CounterClockwise;
            DEBUG_PRINT(" turned counter-clockwise to ");
            degree = (degree - pulseDegree + 360) % 360;
        }
        DEBUG_PRINTLN(degree);        
        for (int i = 0; i < updateCallbackCount; i++) {
            updateCallbacks[i](event, degree);
        }
        loopsSinceLastUpdate = 0;
        return true;
    }
    return false;
}

void Knob::RegisterUpdateCallback(KnobUpdateCallback callback) {
    if (updateCallbackCount >= MAX_KNOB_UPDATE_CALLBACK_COUNT) {
        CONSOLE_PRINT("Knob ");
        CONSOLE_PRINT(knobName);
        CONSOLE_PRINT(" already has ");
        CONSOLE_PRINT(updateCallbackCount);
        CONSOLE_PRINTLN(" update callbacks. Cannot register more.");
        return;
    }
    updateCallbacks[updateCallbackCount] = callback;
    updateCallbackCount++;
}