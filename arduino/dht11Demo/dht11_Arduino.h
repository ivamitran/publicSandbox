#ifndef DHT11_ARDUINO_H
#define DHT11_ARDUINO_H

#define DHT_TIMEOUT 90

// read return flags of sensor
#define DHTLIB_OK               0
#define DHTLIB_ERROR_TIMEOUT    -1
#define DHTLIB_ERROR_CHECKSUM   -2

int readSensor(uint8_t dht11Pin, uint8_t* bytes);
int processDHT_Reading(uint8_t* bytes, double* hum, double* temp);

#endif