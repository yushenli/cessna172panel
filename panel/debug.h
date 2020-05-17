/*
    This header file provides the common text debugging macros using Serial.print.
    All functions can be enabled/disabled by defining/undefing IS_DEBUG.
*/
#ifndef _DEBUG_H
#define _DEBUG_H

// Change this section to configure which serial (over USB) port should be used for console logging
// and talking to the simulator (e.g. spad.neXt) applications.
// #define CONSOLE_ON_SERIAL0
// #define CONSOLE_ON_SERIAL1
#define SIMULATOR_ON_SERIAL0
// #define SIMULATOR_ON_SERIAL1
// #define IS_DEBUG

#if defined CONSOLE_ON_SERIAL0 && defined SIMULATOR_ON_SERIAL0
#error "You cannot have both console and simulator talking on Serial 0 port."
#endif

#if defined CONSOLE_ON_SERIAL1 && defined SIMULATOR_ON_SERIAL1
#error "You cannot have both console and simulator talking on Serial 1 port."
#endif

#if defined CONSOLE_ON_SERIAL0 && defined CONSOLE_ON_SERIAL1
#warning "You cannot send console logs on both Serial 0 and 1 ports."
#endif

#if defined SIMULATOR_ON_SERIAL0 && defined SIMULATOR_ON_SERIAL1
#warning "You cannot talk to the simulator on both Serial 0 and 1 ports."
#endif

#if !defined SIMULATOR_ON_SERIAL0 && !defined SIMULATOR_ON_SERIAL1
#warning "This Arduino is not going to talk to the simulator at all. Are you sure?"
#endif

#ifdef CONSOLE_ON_SERIAL0
#define CONSOLE_SERIAL Serial
#elif defined CONSOLE_ON_SERIAL1
#define CONSOLE_SERIAL Serial1
#endif

#ifdef CONSOLE_SERIAL
#define CONSOLE_PRINT(s) CONSOLE_SERIAL.print(s)
#define CONSOLE_PRINTLN(s) CONSOLE_SERIAL.println(s)
#else
#define CONSOLE_PRINT(s)
#define CONSOLE_PRINTLN(s)
#endif

#ifdef IS_DEBUG
#define DEBUG_PRINT(s) CONSOLE_PRINT(s)
#define DEBUG_PRINTLN(s) CONSOLE_PRINTLN(s)
#else
#define DEBUG_PRINT(s)
#define DEBUG_PRINTLN(s)
#endif

#endif  // _DEBUG_H
