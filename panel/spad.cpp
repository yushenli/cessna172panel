#include "spad.h"

#include "debug.h"

#if defined SIMULATOR_ON_SERIAL0
#define SIMULATOR_SERIAL Serial
#elif defined SIMULATOR_ON_SERIAL1
#define SIMULATOR_SERIAL Serial1
#endif

namespace SPAD {

CmdMessenger* _m;
const char* _deviceName;
bool initialConfigRequestReceived = false;

void Init(const char* deviceName) {
#ifdef SIMULATOR_SERIAL
    _m = new CmdMessenger(SIMULATOR_SERIAL);
    _m->attach(cbUnknownCommand);
    _m->attach(CMDID_FROM_SPAD, cbRequestFromSPAD);
#else
    _m = NULL;
#endif

    _deviceName = deviceName;

    CONSOLE_PRINT("CmdMessenger initialized, device name: ");
    CONSOLE_PRINT(deviceName);
#ifdef SIMULATOR_SERIAL
    CONSOLE_PRINTLN(".");
#else
    CONSOLE_PRINTLN(". Warning: no serial port used to talk to simulator");
#endif
}

void HandleSerialData() {
    if (!_m) {
        return;
    }
    _m->feedinSerialData();
}

void cbUnknownCommand() {
    log("Unknown command received");
}

void handleInitRequest() {
    // For some reason, the CMDID of the first ever command sent by CmdMessenger always gets seen
    // by spad.neXt as an empty char. So sending CMDID_FROM_SPAD directly to serial here.
    SIMULATOR_SERIAL.print("0");
    _m->sendCmdStart(CMDID_FROM_SPAD);
    _m->sendCmdArg("SPAD");
    // TODO: create another library that generate unique GUID and persist in EEPROM
    _m->sendCmdArg("{11111111-2222-3333-4444-000000000000}");
    _m->sendCmdArg(_deviceName);
    _m->sendCmdEnd();
}

void handlePingRequest() {
    _m->sendCmdStart(CMDID_FROM_SPAD);
    _m->sendCmdArg("PONG");
    _m->sendCmdArg(_m->readInt32Arg());
    _m->sendCmdEnd();
}

void handleConfigRequest() {
    _m->sendCmdStart(CMDID_TO_SPAD);
    _m->sendCmdArg("ADD");
    _m->sendCmdArg(10);
    _m->sendCmdArg("leds/systemled"); // will become "SERIAL:<guid>/leds/systemled"
    _m->sendCmdArg("U8");
    _m->sendCmdArg("RW");
    _m->sendCmdArg("SYSTEM_LED");
    _m->sendCmdArg("Toggle LED on/off");
    _m->sendCmdEnd();

    _m->sendCmd(CMDID_FROM_SPAD, "CONFIG");
    initialConfigRequestReceived = true;
}

void cbRequestFromSPAD() {
    const char *requestName = _m->readStringArg();

    if (strcmp(requestName, "INIT") == 0) {
        handleInitRequest();
    }
    else if (strcmp(requestName, "PING") == 0) {
        handlePingRequest();
    }
    else if (strcmp(requestName, "CONFIG") == 0) {
        handleConfigRequest();
    }
}

void log(const char* str) {
    _m->sendCmd(CMDID_DEBUG, str);
}

}  // namespace SPAD
