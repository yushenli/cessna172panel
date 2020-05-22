/*
    InstrumentAdapterSPAD is an InstrumentAdapter implementation that maps events from an instrument
    to the simulator via spad.neXt.
*/

#ifndef _INSTRUMENT_ADAPTER_SPAD_H
#define _INSTRUMENT_ADAPTER_SPAD_H

#include "instrument.h"
#include "instrument_adapter.h"

class InstrumentAdapterSPAD : public InstrumentAdapter {
    public:
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
    protected:
        virtual void registerInstrument(Instrument* instrument);
};

#endif  // _INSTRUMENT_ADAPTER_SPAD_H