#include <Arduino.h>
#include "dht11_Arduino.h"
#include "debug_print.h"

// Define your data pin here
#define DHT_PIN 2

// Define global variables
uint8_t bytes[5];

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
  int outcome = readSensor(DHT_PIN, bytes);
  if(outcome == DHTLIB_OK)
  {
    Serial.print("Raw value read from DHT: ");
    for(int i = 0; i < 5; i++)
    {
      if (bytes[4 - i] < 0x10) Serial.print("0");  // add leading zero if < 16
      Serial.print(bytes[4 - i], HEX);
      Serial.print(" ");
    }
    Serial.println("...");

    double hum, temp;
    outcome = processDHT_Reading(bytes, &hum, &temp);

    if(outcome == DHTLIB_OK)
    {
      Serial.print("Temp: ");
      Serial.print(temp);
      Serial.print(", ");
      Serial.print("Hum: ");
      Serial.println(hum);
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


  delay(5000);
}

/* EOF */