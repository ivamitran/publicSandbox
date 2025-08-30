#include <Arduino.h>
#include "dht11_Arduino.h"
#include "LowPower.h"
#include "debug_print.h"

// Define your data pin here
#define DHT_PIN 2

// Define global variables
uint8_t bytes[5];
uint8_t ranOnce = 0;

void setup() 
{
  Serial.begin(115200);
  while (!Serial)
  {
    // do nothing, just waiting until serial has finished initializing
  }
  debugPrint("Starting program...");
  delay(50);
}

void loop() 
{

  if(ranOnce == 0)
  {
    ranOnce = 1;

    int outcome = readSensor(DHT_PIN, bytes);
    if(outcome == DHTLIB_OK)
    {
      double hum, temp;
      outcome = processDHT_Reading(bytes, &hum, &temp);

      if(outcome == DHTLIB_OK)
      {
        Serial.println("Temp: " + String(temp) + ", Hum: " + String(hum));
      }
      else if(outcome == DHTLIB_ERROR_CHECKSUM)
      {
        Serial.println("Read value invalid, checksum failed...");
      }
    }
    else if(outcome == DHTLIB_ERROR_TIMEOUT)
    {
      Serial.println("Timeout error...");
    }

    delay(1000); // before going to sleep, allow enough time for the serial data to be sent
    return;
  }

  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_ON);
  return;
}

/* EOF */