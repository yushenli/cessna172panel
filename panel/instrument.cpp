#include "instrument.h"

#include "debug.h"

Instrument::Instrument(const char* name, bool isSynced = false) : name_(name),
                                                                  isSynced_(isSynced) {
    DEBUG_PRINT("Instrument ");
    DEBUG_PRINT(name_);
    DEBUG_PRINTLN(" created.");
}

void Instrument::IntValueIncrease() {
    DEBUG_PRINT("Instrument ");
    DEBUG_PRINT(name_);
    DEBUG_PRINTLN(" int value increased with no specific value.");

    for (int i = 0; i < intValueIncreaseCallbackCount_; i++) {
        intValueIncreaseCallbacks_[i]();
    }
}

void Instrument::RegisterIntValueIncrease(InstrumentIntValueIncreaseCallback callback) {    
    if (intValueIncreaseCallbackCount_ >= kMaxInstrumentCallCount) {
        CONSOLE_PRINT("Instrument ");
        CONSOLE_PRINT(name_);
        CONSOLE_PRINT(" already has ");
        CONSOLE_PRINT(intValueIncreaseCallbackCount_);
        CONSOLE_PRINTLN(" IntValueIncrease callbacks. Cannot register more.");
        return;
    }
    intValueIncreaseCallbacks_[intValueIncreaseCallbackCount_] = callback;
    intValueIncreaseCallbackCount_++;
}

void Instrument::IntValueDecrease() {
    DEBUG_PRINT("Instrument ");
    DEBUG_PRINT(name_);
    DEBUG_PRINTLN(" int value decreased with no specific value.");

    for (int i = 0; i < intValueDecreaseCallbackCount_; i++) {
        intValueDecreaseCallbacks_[i]();
    }
}

void Instrument::RegisterIntValueDecrease(InstrumentIntValueDecreaseCallback callback) {    
    if (intValueDecreaseCallbackCount_ >= kMaxInstrumentCallCount) {
        CONSOLE_PRINT("Instrument ");
        CONSOLE_PRINT(name_);
        CONSOLE_PRINT(" already has ");
        CONSOLE_PRINT(intValueDecreaseCallbackCount_);
        CONSOLE_PRINTLN(" IntValueDecrease callbacks. Cannot register more.");
        return;
    }
    intValueDecreaseCallbacks_[intValueDecreaseCallbackCount_] = callback;
    intValueDecreaseCallbackCount_++;
}

// The following section provides some predefined instruments.
// Instruments are provided by groups, different aircrafts may choose the combination
// of different groups.

Instrument** CreateSixPackInstruments(int* size) {
    const int instruments_size = 3;
    Instrument** instruments = new Instrument*[instruments_size];

    instruments[0] = new Instrument("HeadingAdjust");
    instruments[1] = new Instrument("HeadingBug");
    instruments[2] = new Instrument("Altimeter");

    *size = instruments_size;
    return instruments;
}

Instrument** CreateNavigationInstruments(int* size) {
    const int instruments_size = 3;
    Instrument** instruments = new Instrument*[instruments_size];

    instruments[0] = new Instrument("VOR1");
    instruments[1] = new Instrument("VOR2");
    instruments[2] = new Instrument("ADF1");

    *size = instruments_size;
    return instruments;
}
