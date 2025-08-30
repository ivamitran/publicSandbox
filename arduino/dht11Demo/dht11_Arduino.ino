#include "dht11_Arduino.h"
#include <Arduino.h>

int readSensor(uint8_t dht11Pin, uint8_t* bytes)
{
	int mask = 0x80; // mask is 1000 0000
	int byteIndex = 0; // byte index, to keep track of which byte we're on as we iterate through 40 bits serially
	int i;
	int32_t t_0; // used to set an initial time to keep track of elapsed time
    int32_t timeoutThreshold;

    // prepare the bits array for masking, have to do this or else the masking using bitwise OR won't work
	for (i = 0; i < 5; i++)
	{
		bytes[i] = 0;
	}

    // pull the line LOW for 18ms to prompt dht11 to send bit stream
    pinMode(dht11Pin, OUTPUT);
	digitalWrite(dht11Pin, LOW);
	delay(20);

    // reconfig the pin for input to prep for the bit stream
    pinMode(dht11Pin, INPUT_PULLUP);

    // wait for the dht11 to pull the line LOW
    timeoutThreshold = 50;
    t_0 = micros();
    while(digitalRead(dht11Pin) == HIGH)
    {
        if((micros() - t_0) > timeoutThreshold)
		{
			return DHTLIB_ERROR_TIMEOUT;   // timeout
		}
    }

	// after prompting the dht11 to send data, it pulls the line LOW, wait for it to go HIGH again
	timeoutThreshold = DHT_TIMEOUT;
	t_0 = micros();
	while(digitalRead(dht11Pin) == LOW)
	{
		if((micros() - t_0) > timeoutThreshold)
		{
			return DHTLIB_ERROR_TIMEOUT;   // timeout
		}
	}

    // dht11 pulls line HIGH, after this, it starts sending the bit stream
	timeoutThreshold = DHT_TIMEOUT;
	t_0 = micros();
	while(digitalRead(dht11Pin) == HIGH)
	{
		if((micros() - t_0) > timeoutThreshold)
		{
			return DHTLIB_ERROR_TIMEOUT;   // timeout
		}
	}

	// at this point, data is ready to be sent, 40 bits/5 bytes of information is sent in one session
	for(i = 0; i < 40; i++)
	{
		// this is the LOW signal that precedes every bit, wait for it to elapse
		timeoutThreshold = 80;
		t_0 = micros();
		while(digitalRead(dht11Pin) == LOW)
		{
			if((micros() - t_0) > timeoutThreshold)
			{
				// Serial.print("Timed out on bit ");
                // Serial.print(i);
                // Serial.println(", LOW portion...");
				return DHTLIB_ERROR_TIMEOUT; // timeout
			}
		}

		// this is the HIGH signal that represents the bit (the length of this pulse determines 0 or 1)
        // low: ~26-28 us
        // high: ~70 us
        // good separation to more accurately determine LOW or HIGH
		timeoutThreshold = DHT_TIMEOUT;
		t_0 = micros();
		while(digitalRead(dht11Pin) == HIGH)
		{
			if((micros() - t_0) > timeoutThreshold)
			{
				// Serial.print("Timed out on bit ");
                // Serial.print(i);
                // Serial.println(", HIGH portion...");
				return DHTLIB_ERROR_TIMEOUT; //time out
			}
		}

		if((micros() - t_0) > 60)
		{
			bytes[byteIndex] |= mask;  //get the 1 in 1000 000 
		}
		
		mask = mask >> 1; // right shift the 1 bit mask
		if(mask == 0) // if it is 0, means that this byte is done so moving onto next byte
		{
			mask = 0x80; // reset the mask for the next byte
			byteIndex++; // move to next byte
		}
	}

	return DHTLIB_OK;
}

int processDHT_Reading(uint8_t* bytes, double* hum, double* temp)
{
    double humidity = bytes[0];
	double temperature = bytes[2] + bytes[3] * 0.1;
	uint8_t sum = bytes[0] + bytes[1] + bytes[2] + bytes[3];

    if(bytes[4] != sum)
	{
		return DHTLIB_ERROR_CHECKSUM;
	}
    else
    {
        *hum = humidity;
        *temp = temperature;
        return DHTLIB_OK;
    }
}