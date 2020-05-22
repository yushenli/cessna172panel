/*
    InstrumentAdapter defines an interface to map instruments to simulator in some way.
    The concreate classes can implement such mapping, like via spad.neXt or as a joystick device.
*/

#ifndef _INSTRUMENT_ADAPTER_H
#define _INSTRUMENT_ADAPTER_H

#include "instrument.h"

class InstrumentAdapter {
    public:
        virtual void RegisterInstruments(Instrument** instruments, int size);
    protected:
        virtual void registerInstrument(Instrument* instrument) = 0;
};

#endif  // _INSTRUMENT_ADAPTER_H