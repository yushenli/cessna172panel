// This file provides the utility to generate GUIDs and persist them
// for this Arduino device.

#ifndef _GUID_H
#define _GUID_H

namespace GUID {

const int kGUIDLen = 36;

// Gernates a new GUID following RFC4122#4.4
void generateGUID(char* str);

// Get the GUID for the current device for the given slot from EEPROM.
// At most 4 slots can be used.
void GetGUID(char* str, int slot, bool forceGenerate = false);

}

#endif  // _GUID_H