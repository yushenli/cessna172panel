#include "guid.h"

#include "Arduino.h"
#include "EEPROM.h"
#include "../../debug.h"

namespace GUID {

    
const char* kEEPROMPrefix = "C1PN";

void generateGUID(char* str) {
    int r;
    randomSeed(analogRead(0));

    for (int i = 0; i < kGUIDLen; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            str[i] = '-';
            continue;
        }
        r = random(16);

        // Two special handling according to
        // https://tools.ietf.org/html/rfc4122#section-4.4
        if (i == 14) {
            r = 0x04;
        }
        if (i == 19) {
            r = 0x08 | (r & 0x03);
        }

        if (r < 10) {
            str[i] = '0' + r;
        }
        else {
            str[i] = 'A' + r - 10;
        }
    }
}

void GetGUID(char* str, int slot, bool forceGenerate = false) {
    int addr = slot * (strlen(kEEPROMPrefix) + kGUIDLen);

    bool needGenerate = forceGenerate;

    if (!needGenerate) {
        int i = 0;
        for (; i < strlen(kEEPROMPrefix); i++) {
            str[i] = EEPROM.read(addr + i);
        }
        str[i] = '\0';
        needGenerate = strcmp(str, kEEPROMPrefix) != 0;
    }

    if (needGenerate) {        
        generateGUID(str);
        for (int i = 0; i < kGUIDLen; i++) {
            EEPROM.write(strlen(kEEPROMPrefix) + addr + i, str[i]);
        }
        for (int i = 0; i < strlen(kEEPROMPrefix); i++) {
            EEPROM.write(addr + i, kEEPROMPrefix[i]);
        }        
        DEBUG_PRINT("Generating GUID for slot ");
        DEBUG_PRINT(slot);
        DEBUG_PRINT(": ");
        DEBUG_PRINTLN(str);
    }
    else {        
        int i = 0;
        for (; i < kGUIDLen; i++) {
            str[i] = EEPROM.read(strlen(kEEPROMPrefix) + addr + i);
        }
        str[i] = '\0';        
        DEBUG_PRINT("Loaded saved GUID for slot ");
        DEBUG_PRINT(slot);
        DEBUG_PRINT(": ");
        DEBUG_PRINTLN(str);
    }
}

}