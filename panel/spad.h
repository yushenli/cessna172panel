// This file handles communications between this Arduino panel and SPAD.neXt
// SPAD.neXt version >= 0.9.7.5 is required.

#ifndef _SPAD_H
#define _SPAD_H

#include <Arduino.h>
// https://github.com/thijse/Arduino-CmdMessenger is used to handle serial over USB communications
#include <CmdMessenger.h>

class SPAD {

    public:
        // In theory we can have multiple SPAD instances talking to the PC via different serial ports.
        // However, CmdMessenger only takes C style function pointers when attaching callbacks, thus
        // we cannot register SPAD member functions as callbacks.
        // As a workaround I'm making SPAD singleton here, but in a hacky way. The first time you
        // call GetInstance(), you need to pass the necessary parameters. After that you can call
        // GetInstance() with or without the parameters. But if you call it with parameters but
        // they're different from the initial call, a fatal error will be thrown.
        static SPAD* GetInstance();
        static SPAD* GetInstance(const char* deviceGUID, const char* deviceName, Stream& stream);
        void HandleSerialData();
        void SendSimulationEvent(const char* event);

    protected:
        enum cmdid {
            kCmdidFromSPAD   = 0,  // Request from SPAD.neXt
            kCmdidToSPAD     = 1,  // Command to SPAD.neXt
            kCmdidEvents     = 2,  // Events from SPAD.neXt
            kCmdidDebug      = 3,  // Debug strings to SPAD.neXt Logfile
            kCmdidSimulation = 4   // Send Event to Simulation
        };

        SPAD(const char* deviceGUID, const char* deviceName, Stream& stream);
        static void cbUnknownCommand();
        static void cbRequestFromSPAD();
        void handleInitRequest();
        void handlePingRequest();
        void handleConfigRequest();

        void log(const char* str);        

        Stream* stream_;
        CmdMessenger* messenger_;
        const char* deviceGUID_;
        const char* deviceName_;
        bool initialConfigRequestReceived_ = false;
};

#endif  // _SPAD_H
