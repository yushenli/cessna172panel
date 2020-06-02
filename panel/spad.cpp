#include "spad.h"

#include "debug.h"

static SPAD* singleton_spad_ = nullptr;

static SPAD* SPAD::GetInstance() {
    return singleton_spad_;
}

static SPAD* SPAD::GetInstance(const char* deviceGUID, const char* deviceName, Stream& stream) {
    if (!singleton_spad_) {
        singleton_spad_ = new SPAD(deviceGUID, deviceName, stream);
    }
    else {
        if (strcmp(deviceGUID, singleton_spad_->deviceGUID_)
            || strcmp(deviceName, singleton_spad_->deviceName_)
            || &stream != singleton_spad_->stream_) {
            CONSOLE_PRINTLN(
                "FATAL: SPAD is singleton but SPAD::GetInstance() was called with different parameters.");
        }
    }
    return singleton_spad_;
}

SPAD::SPAD(const char* deviceGUID, const char* deviceName, Stream& stream) {
    stream_ = &stream;
    messenger_ = new CmdMessenger(stream);
    messenger_->attach(cbUnknownCommand);
    messenger_->attach(kCmdidFromSPAD, cbRequestFromSPAD);

    deviceGUID_ = deviceGUID;
    deviceName_ = deviceName;

    CONSOLE_PRINT("CmdMessenger initialized, device name: ");
    CONSOLE_PRINT(deviceName_);
    CONSOLE_PRINT(", GUID: ");
    CONSOLE_PRINT(deviceGUID_);
    CONSOLE_PRINTLN(".");
}

void SPAD::HandleSerialData() {
    messenger_->feedinSerialData();
}

void SPAD::SendSimulationEvent(const char* event) {
    messenger_->sendCmd(kCmdidSimulation, event);
}

static void SPAD::cbUnknownCommand() {
    GetInstance()->log("Unknown command received");
}

void SPAD::handleInitRequest() {
    // For some reason, the CMDID of the first ever command sent by CmdMessenger always gets seen
    // by spad.neXt as an empty char. So sending kCmdidFromSPAD directly to serial here.
    stream_->print("0");
    messenger_->sendCmdStart(kCmdidFromSPAD);
    messenger_->sendCmdArg("SPAD");
    messenger_->sendCmdArg(deviceGUID_);
    messenger_->sendCmdArg(deviceName_);
    messenger_->sendCmdEnd();
}

void SPAD::handlePingRequest() {
    messenger_->sendCmdStart(kCmdidFromSPAD);
    messenger_->sendCmdArg("PONG");
    messenger_->sendCmdArg(messenger_->readInt32Arg());
    messenger_->sendCmdEnd();
}

void SPAD::handleConfigRequest() {
    messenger_->sendCmdStart(kCmdidToSPAD);
    messenger_->sendCmdArg("ADD");
    messenger_->sendCmdArg(10);
    messenger_->sendCmdArg("leds/systemled"); // will become "SERIAL:<guid>/leds/systemled"
    messenger_->sendCmdArg("U8");
    messenger_->sendCmdArg("RW");
    messenger_->sendCmdArg("SYSTEM_LED");
    messenger_->sendCmdArg("Toggle LED on/off");
    messenger_->sendCmdEnd();

    messenger_->sendCmd(kCmdidFromSPAD, "CONFIG");
    initialConfigRequestReceived_ = true;
}

static void SPAD::cbRequestFromSPAD() {    
    const char *requestName = GetInstance()->messenger_->readStringArg();

    if (strcmp(requestName, "INIT") == 0) {
         GetInstance()->handleInitRequest();
    }
    else if (strcmp(requestName, "PING") == 0) {
         GetInstance()->handlePingRequest();
    }
    else if (strcmp(requestName, "CONFIG") == 0) {
         GetInstance()->handleConfigRequest();
    }
}

void SPAD::log(const char* str) {
    messenger_->sendCmd(kCmdidDebug, str);
}
