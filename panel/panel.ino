#include "debug.h"
#include "instrument.h"
#include "instrument_adapter.h"
#include "instrument_adapter_spad.h"
#include "spad.h"
#include "src/device/guid.h"
#include "src/hardware/knob.h"
#include "src/hardware/guitar_pickup.h"

const uint32_t kUpdateStateLengthUs           = 2000000;
const uint32_t kCheckIntervalUsUpdateState    =     100;
const uint32_t kCheckIntervalUsNonUpdateState =    5000;  // 5 miliseconds

enum knobIndex {
    kKnobIndexHeadingAdjust = 0,
    kKnobIndexHeadingBug    = 1,
    kKnobIndexVOR2          = 2,
    kKnobIndexAltimeter     = 3,
    kKnobIndexVOR1          = 4
};
const struct {
    const char* name;
    const int pinClk;
    const int pinDt;
} kKnobsSpecs[] = {
    // Lower level knobs:
    {"HeadingAdjust", 53, 52},
    {"HeadingBug", 51, 50},
    {"VOR2", 49, 48},
    // Upper level knobs:
    {"Altimeter", 4, 5},
    {"VOR1", 2, 3}
};

const char* kFlapGuitarPickupName = "FlapSelector";
const int kFlapGuitarPickupPins[3] = {23, 25, 27};

// You can configure what instruments the aircraft shall have by editting
// this const array to change.
const Instrument** (*kCessna172InstrumentGroups[])(int*) = {
    CreateSixPackInstruments,
    CreateNavigationInstruments
};

Knob** knobs;
GuitarPickup* guitarPickup;
Instrument** instruments;
SPAD* spad = nullptr;
int instrumentsCount;
InstrumentAdapter* instrumentAdapter = nullptr;
int loopIntervalUs;
int updateStateEndsInChecks;
char spadGUID[GUID::kGUIDLen + 1];

void knobToInstrumentCallback(Instrument* p, const char* instrumentName, Knob* knob, bool isClockwiseDec = false) {
    if (!strcmp(p->GetName(), instrumentName)) {
        if (isClockwiseDec) {
            knob->RegisterUpdateCallback(
                [p](KnobUpdateEvent event, int degree) {
                    if (event == Clockwise) {
                        p->IntValueDecrease();
                    } else if (event == CounterClockwise) {
                        p->IntValueIncrease();
                    }
                });
        }
        else {
            knob->RegisterUpdateCallback(
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
    const int count = sizeof(kKnobsSpecs) / sizeof(kKnobsSpecs[0]);
    knobs = new Knob*[count];
    DEBUG_PRINT("Total number of knobs defined: ");
    DEBUG_PRINTLN(count);
    for (int i = 0; i < count; i++) {
        knobs[i] = new Knob(kKnobsSpecs[i].name, kKnobsSpecs[i].pinClk, kKnobsSpecs[i].pinDt, 12);
    }
}

// Creates all guitar pickup supported by the hardware.
void initGuitarPickup() {
    guitarPickup = new GuitarPickup(kFlapGuitarPickupName, kFlapGuitarPickupPins);
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
            knobToInstrumentCallback(*p, "HeadingAdjust", knobs[kKnobIndexHeadingAdjust]);
            knobToInstrumentCallback(*p, "HeadingBug", knobs[kKnobIndexHeadingBug]);
            knobToInstrumentCallback(*p, "Altimeter", knobs[kKnobIndexAltimeter]);
            knobToInstrumentCallback(*p, "VOR1", knobs[kKnobIndexVOR1], true);
            knobToInstrumentCallback(*p, "VOR2", knobs[kKnobIndexVOR2], true);
            p++;
        }
    }
}

// the setup function runs once when you press reset or power the board
void setup() {
#ifdef CONSOLE_SERIAL
    CONSOLE_SERIAL.begin(115200);
#endif

    loopIntervalUs = 5000;
    updateStateEndsInChecks = 0;
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    initKnobs();
    initGuitarPickup();
    initInstruments();

#if defined SIMULATOR_ON_SERIAL0
    Serial.begin(115200);
    GUID::GetGUID(spadGUID, 0);
    spad = SPAD::GetInstance(spadGUID, "Cessna 172 Instrument Panel", Serial);
#elif defined SIMULATOR_ON_SERIAL1
    Serial1.begin(115200);
    GUID::GetGUID(spadGUID, 1);
    spad = SPAD::GetInstance(spadGUID, "Cessna 172 Instrument Panel", Serial1);
#else
    CONSOLE_PRINTLN("Warning: no serial port used to talk to simulator");
#endif

    if (spad) {
        instrumentAdapter = new InstrumentAdapterSPAD(spad);
        instrumentAdapter->RegisterInstruments(instruments, instrumentsCount);
    }
}

// the loop function runs over and over again forever
void loop() {
    bool updated = false;
    for (int i = 0; i < sizeof(kKnobsSpecs) / sizeof(kKnobsSpecs[0]); i++) {
        updated |= knobs[i]->Update();
    }
    updated |= guitarPickup->Update();

    if (updated) {
        if (updateStateEndsInChecks <= 0) {
            digitalWrite(LED_BUILTIN, HIGH);
            loopIntervalUs = kCheckIntervalUsUpdateState;
        }
        updateStateEndsInChecks = kUpdateStateLengthUs / kCheckIntervalUsUpdateState;
    }
    else {
        updateStateEndsInChecks--;
        if (updateStateEndsInChecks <= 0) {
            digitalWrite(LED_BUILTIN, LOW);
            loopIntervalUs = kCheckIntervalUsNonUpdateState;
        }
    }

    if (spad) {
        spad->HandleSerialData();
    }

    delayMicroseconds(loopIntervalUs);
}
