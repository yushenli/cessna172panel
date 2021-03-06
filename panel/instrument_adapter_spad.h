/*
    InstrumentAdapterSPAD is an InstrumentAdapter implementation that maps events from an instrument
    to the simulator via spad.neXt.
*/

#ifndef _INSTRUMENT_ADAPTER_SPAD_H
#define _INSTRUMENT_ADAPTER_SPAD_H

#include "instrument.h"
#include "instrument_adapter.h"
#include "spad.h"

class InstrumentAdapterSPAD : public InstrumentAdapter {
    public:
        InstrumentAdapterSPAD(SPAD* spad);
        void OnAltimeterIntValueIncrease();
        void OnAltimeterIntValueDecrease();
        void OnHeadingAdjustIntValueIncrease();
        void OnHeadingAdjustIntValueDecrease();
        void OnHeadingBugIntValueIncrease();
        void OnHeadingBugIntValueDecrease();
        void OnVOR1IntValueIncrease();
        void OnVOR1IntValueDecrease();
        void OnVOR2IntValueIncrease();
        void OnVOR2IntValueDecrease();
        void OnADF1IntValueIncrease();
        void OnADF1IntValueDecrease();
        void OnFlapIntValueSet(int value);
    protected:
        virtual void registerInstrument(Instrument* instrument);

        SPAD* spad_;
};

#endif  // _INSTRUMENT_ADAPTER_SPAD_H