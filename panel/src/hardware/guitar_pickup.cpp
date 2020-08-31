#include "guitar_pickup.h"

#include "../../debug.h"

GuitarPickup::GuitarPickup(const char* guitarPickupName, int pins[3]) {
    this->guitarPickupName_ = guitarPickupName;
    memcpy(pins_, pins, sizeof(pins_));

    for (int i = 0; i < sizeof(pins_) / sizeof(int); i++) {
        pinMode(pins_[i], INPUT_PULLUP);
    }

    CONSOLE_PRINT("Initialized guitar pick ");
    CONSOLE_PRINT(this->guitarPickupName_);
    CONSOLE_PRINT(" PINS=");
    for (int i = 0; i < sizeof(pins_) / sizeof(int); i++) {
        CONSOLE_PRINT(pins_[i]);
        CONSOLE_PRINT(",");
    }
    CONSOLE_PRINTLN("");
}

bool GuitarPickup::Update() {
    int grayCode = 0;
    for (int i = sizeof(pins_) / sizeof(int) - 1; i >= 0; i--) {
        // Mark a PIN's corresponding bit as 1 if it's connected to the GND pin.
        // PIN 0's value at lowest bit.
        grayCode <<= 1;
        grayCode |= digitalRead(pins_[i]) == LOW ? 1 : 0;
    }
    if (grayCode != lastGrayCode_) {
        DEBUG_PRINT("Read Gray code for guitar pickup ");
        DEBUG_PRINT(guitarPickupName_);
        DEBUG_PRINT(" last ");
        DEBUG_PRINT(lastGrayCode_);
        DEBUG_PRINT(" now ");
        DEBUG_PRINTLN(grayCode);

        lastGrayCode_ = grayCode;

        int position = -1;
        switch (grayCode) {
            case 0b00000100:
                position = 0;
                break;
            case 0b00000110:
                position = 1;
                break;
            case 0b00000010:
                position = 2;
                break;
            case 0b00000011:
                position = 3;
                break;
            case 0b00000001:
                position = 4;
                break;
        }

        if (position == -1) {
            CONSOLE_PRINT("Guitar pickup ");
            CONSOLE_PRINT(guitarPickupName_);
            CONSOLE_PRINT(" reads invalid Gray Code ");
            CONSOLE_PRINTLN(grayCode);
            return false;
        }
        else {            
            DEBUG_PRINT("Guitar pickup ");
            DEBUG_PRINT(guitarPickupName_);
            DEBUG_PRINT(" updated, new Gray code: ");
            DEBUG_PRINT(grayCode);
            DEBUG_PRINT(" , new position: ");
            DEBUG_PRINTLN(position);
        }

        for (int i = 0; i < updateCallbackCount_; i++) {
            updateCallbacks_[i](position);
        }

        return true;
    }
    return false;
}

void GuitarPickup::RegisterUpdateCallback(GuitarPickupUpdateCallback callback) {
    if (updateCallbackCount_ >= kMaxGuitarPickupUpdateCallbackCount) {
        CONSOLE_PRINT("Guitar pick ");
        CONSOLE_PRINT(guitarPickupName_);
        CONSOLE_PRINT(" already has ");
        CONSOLE_PRINT(updateCallbackCount_);
        CONSOLE_PRINTLN(" update callbacks. Cannot register more.");
        return;
    }
    updateCallbacks_[updateCallbackCount_] = callback;
    updateCallbackCount_++;
}