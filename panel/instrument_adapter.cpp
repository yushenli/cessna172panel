#include "instrument_adapter.h"

void InstrumentAdapter::RegisterInstruments(Instrument** instruments, int size) {
    for (int i = 0; i < size; i++) {
        registerInstrument(instruments[i]);
    }
}
