#include "knob.h"

#define UPDATE_STATE_LENGTH_US             2000000
#define CHECK_INTERVAL_US_UPDATE_STATE         100
#define CHECK_INTERVAL_US_NON_UPDATE_STATE    5000  // 5 miliseconds

#define ALTIMETER                0
#define PIN_ALTIMETER_CLK        23
#define PIN_ALTIMETER_DT         22

Knob* knobs[1];
int loopIntervalUs;
int updateStateEndsInChecks;

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(9600);

    loopIntervalUs = 5000;
    updateStateEndsInChecks = 0;
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    knobs[ALTIMETER] = new Knob("ALTIMETER", PIN_ALTIMETER_CLK, PIN_ALTIMETER_DT, 12);
}

// the loop function runs over and over again forever
void loop() {
    bool updated = false;
    for (int i = 0; i < sizeof(knobs) / sizeof(Knob*); i++) {
        updated |= knobs[i]->Update();
    }

    if (updated) {
        if (updateStateEndsInChecks <= 0) {
            digitalWrite(LED_BUILTIN, HIGH);
            loopIntervalUs = CHECK_INTERVAL_US_UPDATE_STATE;
        }
        updateStateEndsInChecks = UPDATE_STATE_LENGTH_US / CHECK_INTERVAL_US_UPDATE_STATE;
    }
    else {
        updateStateEndsInChecks--;
        if (updateStateEndsInChecks <= 0) {
            digitalWrite(LED_BUILTIN, LOW);
            loopIntervalUs = CHECK_INTERVAL_US_NON_UPDATE_STATE;
        }
    }

    delayMicroseconds(loopIntervalUs);
}
