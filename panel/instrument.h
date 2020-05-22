/*
    This class provides a range of instruments that can be mapped to knobs and buttons,
    each instrument can also optionally track their value in the Simulator.
*/
#ifndef _INSTRUMENT_H
#define _INSTRUMENT_H

#include <Arduino.h>
#include "lib/functional-avr/nonstd.h"

#define MAX_INSTRUMENT_CALLBACK_COUNT 2

typedef nonstd::function<void()> InstrumentIntValueIncreaseCallback;
typedef nonstd::function<void()> InstrumentIntValueDecreaseCallback;

class Instrument {
    public:
        Instrument(const char* name, bool isSynced = false);
        inline bool GetIsSynced() { return isSynced_; }
        inline const char* GetName() { return name_; }
        void IntValueIncrease();
        void RegisterIntValueIncrease(InstrumentIntValueIncreaseCallback callback);
        void IntValueDecrease();
        void RegisterIntValueDecrease(InstrumentIntValueDecreaseCallback callback);
    protected:
        const char* name_;
        // isSynced_ indicates if an adapter is actively syning the internal value with the simulator.
        // If false, the instrument will only report updates to the simulator, but the interval values,
        // such as value_int_ will not be updated according to the simulator.
        bool isSynced_;
        int value_int_ = 0;

        int intValueIncreaseCallbackCount_ = 0;
        InstrumentIntValueIncreaseCallback intValueIncreaseCallbacks_[MAX_INSTRUMENT_CALLBACK_COUNT];
        int intValueDecreaseCallbackCount_ = 0;
        InstrumentIntValueDecreaseCallback intValueDecreaseCallbacks_[MAX_INSTRUMENT_CALLBACK_COUNT];
};

Instrument** CreateSixPackInstruments(int* size);
Instrument** CreateNavigationInstruments(int* size);

#endif  // _INSTRUMENT_H