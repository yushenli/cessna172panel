#include "knob.h"

#define UPDATE_STATE_LENGTH_US             2000000
#define CHECK_INTERVAL_US_UPDATE_STATE         100
#define CHECK_INTERVAL_US_NON_UPDATE_STATE    5000  // 5 miliseconds

#define HDG_ADJUST                0
#define PIN_HDG_ADJUST_CLK       53
#define PIN_HDG_ADJUST_DT        52
#define HDG_BUG                   1
#define PIN_HDG_BUG_CLK          51
#define PIN_HDG_BUG_DT           50
#define VOR2                      2
#define PIN_VOR2_CLK             49
#define PIN_VOR2_DT              48
#define ALTIMETER                 3
#define PIN_ALTIMETER_CLK         4
#define PIN_ALTIMETER_DT          5
#define VOR1                      4
#define PIN_VOR1_CLK              2
#define PIN_VOR1_DT               3

Knob* knobs[5];
int loopIntervalUs;
int updateStateEndsInChecks;

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);

    loopIntervalUs = 5000;
    updateStateEndsInChecks = 0;
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    knobs[HDG_ADJUST] = new Knob("HDG_ADJUST", PIN_HDG_ADJUST_CLK, PIN_HDG_ADJUST_DT, 12);
    knobs[HDG_BUG] = new Knob("HDG_BUG", PIN_HDG_BUG_CLK, PIN_HDG_BUG_DT, 12);
    knobs[VOR2] = new Knob("VOR2", PIN_VOR2_CLK, PIN_VOR2_DT, 12);
    knobs[ALTIMETER] = new Knob("ALTIMETER", PIN_ALTIMETER_CLK, PIN_ALTIMETER_DT, 12);
    knobs[VOR1] = new Knob("VOR1", PIN_VOR1_CLK, PIN_VOR1_DT, 12);
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
