#include "debug.h"
#include "instrument.h"
#include "instrument_adapter.h"
#include "instrument_adapter_spad.h"
#include "knob.h"
#include "spad.h"

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

// You can configure what instruments the aircraft shall have by editting
// this const array to change.
const Instrument** (*kCessna172InstrumentGroups[])(int*) = {
    CreateSixPackInstruments,
    CreateNavigationInstruments
};

Knob* knobs[5];
Instrument** instruments;
int instrumentsCount;
InstrumentAdapter* instrumentAdapter;
int loopIntervalUs;
int updateStateEndsInChecks;

void knobToInstrumentCallback(Instrument* p, const char* instrumentName, int knobIndex, bool isClockwiseDec = false) {
    if (!strcmp(p->GetName(), instrumentName)) {
        if (isClockwiseDec) {
            knobs[knobIndex]->RegisterUpdateCallback(
                [p](KnobUpdateEvent event, int degree) {
                    if (event == Clockwise) {
                        p->IntValueDecrease();
                    } else if (event == CounterClockwise) {
                        p->IntValueIncrease();
                    }
                });
        }
        else {
            knobs[knobIndex]->RegisterUpdateCallback(
                [p](KnobUpdateEvent event, int degree) {
                    if (event == Clockwise) {
                        p->IntValueIncrease();
                    } else if (event == CounterClockwise) {
                        p->IntValueDecrease();
                    }
                });
        }
    }
}

// Creates all the Knobs supported by the hardware.
void initKnobs() {
    knobs[HDG_ADJUST] = new Knob("HDG_ADJUST", PIN_HDG_ADJUST_CLK, PIN_HDG_ADJUST_DT, 12);
    knobs[HDG_BUG] = new Knob("HDG_BUG", PIN_HDG_BUG_CLK, PIN_HDG_BUG_DT, 12);
    knobs[VOR2] = new Knob("VOR2", PIN_VOR2_CLK, PIN_VOR2_DT, 12);
    knobs[ALTIMETER] = new Knob("ALTIMETER", PIN_ALTIMETER_CLK, PIN_ALTIMETER_DT, 12);
    knobs[VOR1] = new Knob("VOR1", PIN_VOR1_CLK, PIN_VOR1_DT, 12);
}

// Creates all the Instruments supported in the simulation aircraft.
void initInstruments() {
    const int numInstrumentGroups = sizeof(kCessna172InstrumentGroups) / sizeof(*kCessna172InstrumentGroups);
    int* groupSizes = new int[numInstrumentGroups];
    Instrument*** instrumentGroups = new Instrument**[numInstrumentGroups];

    instrumentsCount = 0;
    for (int i = 0; i < numInstrumentGroups; i++) {
        instrumentGroups[i] = kCessna172InstrumentGroups[i](groupSizes + i);
        instrumentsCount += groupSizes[i];
    }

    instruments = new Instrument*[instrumentsCount];
    Instrument** p = instruments;

    for (int i = 0; i < numInstrumentGroups; i++) {
        for (int j = 0; j < groupSizes[i]; j++) {
            *p = (instrumentGroups[i])[j];
            knobToInstrumentCallback(*p, "HeadingAdjust", HDG_ADJUST);
            knobToInstrumentCallback(*p, "HeadingBug", HDG_BUG);
            knobToInstrumentCallback(*p, "Altimeter", ALTIMETER);
            knobToInstrumentCallback(*p, "VOR1", VOR1, true);
            knobToInstrumentCallback(*p, "VOR2", VOR2, true);
            p++;
        }
    }
}

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);

    loopIntervalUs = 5000;
    updateStateEndsInChecks = 0;
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    initKnobs();
    initInstruments();

    instrumentAdapter = new InstrumentAdapterSPAD();
    instrumentAdapter->RegisterInstruments(instruments, instrumentsCount);

    SPAD::Init("Cessna 172 Instrument Panel");
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

    SPAD::HandleSerialData();

    delayMicroseconds(loopIntervalUs);
}
