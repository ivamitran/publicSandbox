#include "debug_print.h"
#include "Arduino.h"

void debugPrint(const char* string)
{
    #ifdef DEBUG_PRINT
        Serial.println(string);
    #endif
}