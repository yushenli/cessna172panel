// This file handles communications between this Arduino panel and SPAD.neXt
// SPAD.neXt version >= 0.9.7.5 is required.

#ifndef _SPAD_H
#define _SPAD_H

#include <Arduino.h>
// https://github.com/thijse/Arduino-CmdMessenger is used to handle serial over USB communications
#include <CmdMessenger.h>

namespace SPAD {

enum cmdid {
    kCmdidFromSPAD   = 0,  // Request from SPAD.neXt
    kCmdidToSPAD     = 1,  // Command to SPAD.neXt
    kCmdidEvents     = 2,  // Events from SPAD.neXt
    kCmdidDebug      = 3,  // Debug strings to SPAD.neXt Logfile
    kCmdidSimulation = 4   // Send Event to Simulation
};

void Init(const char* deviceName);
void HandleSerialData();
void SendSimulationEvent(const char* event);

void cbUnknownCommand();
void cbRequestFromSPAD();
void log(const char* str);

}  // namespace SPAD

#endif  // _SPAD_H
