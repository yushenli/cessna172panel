/*
 * This header file provides the common text debugging macros using Serial.print.
 * All functions can be enabled/disabled by defining/undefing IS_DEBUG.
 */
#ifndef _DEBUG_H
#define _DEBUG_H

//#define IS_DEBUG

#ifdef IS_DEBUG
#define DEBUG_PRINT(s) Serial.print(s)
#define DEBUG_PRINTLN(s) Serial.println(s)
#else
#define DEBUG_PRINT(s)
#define DEBUG_PRINTLN(s)
#endif

#endif  // _DEBUG_H
