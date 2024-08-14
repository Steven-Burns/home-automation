#pragma once

#include "Arduino.h"



class DHTAsync {
public:
    DHTAsync(uint8_t pin);

    bool measure(float *temperature, float *humidity, bool autoSync = true);

    void measureSync(float *temperature, float *humidity);

    float convertCtoF(float c);

    float convertFtoC(float f);

private:
    uint8_t dhtState;
    unsigned long dhtTimestamp;
    uint8_t data[5];
    const uint8_t _pin, _type;
#ifdef __AVR
    // Use direct GPIO access on an 8-bit AVR so keep track of the port and
    // bitmask for the digital pin connected to the DHT.  Other platforms will use
    // digitalRead.
    const uint8_t _bit, _port;
#endif
    const uint32_t _maxCycles;

    bool readData();

    bool readAsync();

    float readTemperature() const;

    float readHumidity() const;

    uint32_t expectPulse(bool level) const;
};



