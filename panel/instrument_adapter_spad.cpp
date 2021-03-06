#include "instrument_adapter_spad.h"
#include "debug.h"

InstrumentAdapterSPAD::InstrumentAdapterSPAD(SPAD* spad) {
    spad_ = spad;
}

void InstrumentAdapterSPAD::registerInstrument(Instrument* instrument) {
    if (!strcmp(instrument->GetName(), "Altimeter")) {
        instrument->RegisterIntValueIncrease([this]() { OnAltimeterIntValueIncrease(); });
        instrument->RegisterIntValueDecrease([this]() { OnAltimeterIntValueDecrease(); });
    }
    if (!strcmp(instrument->GetName(), "HeadingAdjust")) {
        instrument->RegisterIntValueIncrease([this]() { OnHeadingAdjustIntValueIncrease(); });
        instrument->RegisterIntValueDecrease([this]() { OnHeadingAdjustIntValueDecrease(); });
    }
    if (!strcmp(instrument->GetName(), "HeadingBug")) {
        instrument->RegisterIntValueIncrease([this]() { OnHeadingBugIntValueIncrease(); });
        instrument->RegisterIntValueDecrease([this]() { OnHeadingBugIntValueDecrease(); });
    }
    if (!strcmp(instrument->GetName(), "VOR1")) {
        instrument->RegisterIntValueIncrease([this]() { OnVOR1IntValueIncrease(); });
        instrument->RegisterIntValueDecrease([this]() { OnVOR1IntValueDecrease(); });
    }
    if (!strcmp(instrument->GetName(), "VOR2")) {
        instrument->RegisterIntValueIncrease([this]() { OnVOR2IntValueIncrease(); });
        instrument->RegisterIntValueDecrease([this]() { OnVOR2IntValueDecrease(); });
    }
    if (!strcmp(instrument->GetName(), "ADF1")) {
        instrument->RegisterIntValueIncrease([this]() { OnADF1IntValueIncrease(); });
        instrument->RegisterIntValueDecrease([this]() { OnADF1IntValueDecrease(); });
    }
    if (!strcmp(instrument->GetName(), "Flap")) {
        instrument->RegisterIntValueSet([this](int value) { OnFlapIntValueSet(value); });
    }
    // It's totally normal if this InstrumentAdapter doesn't recognize the instrument's name.
    // An adapter doesn't have to support all intruments.
}

void InstrumentAdapterSPAD::OnAltimeterIntValueIncrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnAltimeterValueIncrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:KOHLSMAN_INC");
}

void InstrumentAdapterSPAD::OnAltimeterIntValueDecrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnAltimeterValueDecrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:KOHLSMAN_DEC");
}

void InstrumentAdapterSPAD::OnHeadingAdjustIntValueIncrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnHeadingAdjustIntValueIncrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:GYRO_DRIFT_INC");
}

void InstrumentAdapterSPAD::OnHeadingAdjustIntValueDecrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnHeadingAdjustIntValueDecrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:GYRO_DRIFT_DEC");
}

void InstrumentAdapterSPAD::OnHeadingBugIntValueIncrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnHeadingBugIntValueIncrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:HEADING_BUG_INC");
}

void InstrumentAdapterSPAD::OnHeadingBugIntValueDecrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnHeadingBugIntValueDecrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:HEADING_BUG_DEC");
}

void InstrumentAdapterSPAD::OnVOR1IntValueIncrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnVOR1IntValueIncrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:VOR1_OBI_INC");
}

void InstrumentAdapterSPAD::OnVOR1IntValueDecrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnVOR1IntValueDecrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:VOR1_OBI_DEC");
}

void InstrumentAdapterSPAD::OnVOR2IntValueIncrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnVOR2IntValueIncrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:VOR2_OBI_INC");
}

void InstrumentAdapterSPAD::OnVOR2IntValueDecrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnVOR2IntValueDecrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:VOR2_OBI_DEC");
}

void InstrumentAdapterSPAD::OnADF1IntValueIncrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnADF1IntValueIncrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:ADF_CARD_INC");
}

void InstrumentAdapterSPAD::OnADF1IntValueDecrease() {
    DEBUG_PRINTLN("InstrumentAdapterSPAD::OnADF1IntValueDecrease triggered");
    spad_->SendSimulationEvent("SIMCONNECT:ADF_CARD_DEC");
}

void InstrumentAdapterSPAD::OnFlapIntValueSet(int value) {
    DEBUG_PRINT("InstrumentAdapterSPAD::OnFlapIntValueSet triggered with value ");
    DEBUG_PRINTLN(value);
    switch (value) {
        case 0:
            spad_->SendSimulationEvent("SIMCONNECT:FLAPS_UP");
            break;
        case 1:
            spad_->SendSimulationEvent("SIMCONNECT:FLAPS_1");
            break;
        case 2:
            spad_->SendSimulationEvent("SIMCONNECT:FLAPS_2");
            break;
        default:
            spad_->SendSimulationEvent("SIMCONNECT:FLAPS_DOWN");
    }
}
