#include "LowPower.h"

void setup() 
{
    Serial.begin(115200);
    while (!Serial)
    {
        // do nothing, just waiting until serial has finished initializing
    }
}
void loop()
{
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_ON);
    // turn off the internal ADC (i.e., connected to the analog input pins) to save a bit more power
    // keep BOD on for voltage stability
    // With this line: 5V, 44mA
    // Without this line: 5V, 25mA
    // Takeaway: The Arduino Uno dev kit is not optimized for power efficiency, in general, dev kits are meant for prototyping, not power efficiency
}
