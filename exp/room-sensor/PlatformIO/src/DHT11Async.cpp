// This code refurbished and made to work with Raspberry Pi Pico from 
// https://github.com/toannv17/DHT-Sensors-Non-Blocking 
//
// what didn't work with the original:
// - disabling interupts.  Crashes the mbedOS on the Pico
// - requires F_CPU to be defined, which appears to have been removed
// from the Arduino SDK of late.


#include "DHT11Async.hpp"


#define F_CPU 130000000L  // Raspberry Pi Pico v1, according to internet
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )



#define DHT_TYPE_11  0
#define DHT_TYPE_12  1
#define DHT_TYPE_21  2
#define DHT_TYPE_22  3
#define DHT_IDLE                  0
#define DHT_BEGIN_MEASUREMENT     1
#define DHT_BEGIN_MEASUREMENT_2   2
#define DHT_DO_READING            3
#define DHT_COOLDOWN              4

/* Number of milliseconds before a new sensor read may be initiated. */
#define COOLDOWN_TIME  2000


/*
 * Constructor for the sensor.  It remembers the pin number and the
 * type of sensor, and initializes internal variables.
 */
DHTAsync::DHTAsync(uint8_t pin)
        : _pin(pin),
          _type(DHT_TYPE_11),
#ifdef __AVR
          _bit(digitalPinToBitMask(pin)),
          _port(digitalPinToPort(pin)),
#endif
          _maxCycles(microsecondsToClockCycles(1000)) {
    dhtState = DHT_IDLE;

    pinMode(_pin, INPUT);
    digitalWrite(_pin, HIGH);
}

/*
 * Instruct the DHT to begin sampling.  Keep polling until it returns true.
 * The temperature is in degrees Celsius, and the humidity is in %.
 */
bool DHTAsync::measure(float *temperature, float *humidity, bool autoSync) {
    if (autoSync && (
            (dhtState == DHT_BEGIN_MEASUREMENT_2 && millis() - dhtTimestamp > 500) ||
            (dhtState == DHT_DO_READING && millis() - dhtTimestamp > 90)
        )
    ) {
        measureSync(temperature, humidity);
        return true;
    }

    if (readAsync()) {
        *temperature = readTemperature();
        *humidity = readHumidity();
        return true;
    } else {
        return false;
    }
}

void DHTAsync::measureSync(float *temperature, float *humidity) {
    while (1) {
        if (readAsync()) {
            *temperature = readTemperature();
            *humidity = readHumidity();
            return;
        }
    }
}

float DHTAsync::readTemperature() const {
    float f = NAN;

    switch (_type) {
        case DHT_TYPE_11:
            f = data[2];
            if (data[3] & 0x80) {
                f = -1 - f;
            }
            f += (data[3] & 0x0f) * 0.1;
            break;

        case DHT_TYPE_12:
            f = data[2];
            f += (data[3] & 0x0f) * 0.1;
            if (data[2] & 0x80) {
                f *= -1;
            }
            break;

        case DHT_TYPE_21:
        case DHT_TYPE_22:
            f = ((word) (data[2] & 0x7F)) << 8 | data[3];
            f *= 0.1;
            if (data[2] & 0x80) {
                f *= -1;
            }
            break;
    }

    return f;
}


float DHTAsync::readHumidity() const {
    float f = NAN;

    switch (_type) {
        case DHT_TYPE_11:
        case DHT_TYPE_12:
            f = data[0] + data[1] * 0.1;
            break;

        case DHT_TYPE_21:
        case DHT_TYPE_22:
            f = ((word) data[0]) << 8 | data[1];
            f *= 0.1;
            break;
    }

    return f;
}


/*
 * Expect the input to be at the specified level and return the number
 * of loop cycles spent there.  This is identical to Adafruit's blocking
 * driver.
 */
uint32_t DHTAsync::expectPulse(bool level) const {
// F_CPU is not be known at compile time on platforms such as STM32F103.
// The preprocessor seems to evaluate it to zero in that case.
#if (F_CPU > 16000000L) || (F_CPU == 0L)
    uint32_t count = 0;
#else
    uint16_t count = 0; // To work fast enough on slower AVR boards
#endif
    // On AVR platforms use direct GPIO port access as it's much faster and better
    // for catching pulses that are 10's of microseconds in length:
#ifdef __AVR
    uint8_t portState = level ? _bit : 0;
    while ((*portInputRegister(_port) & _bit) == portState) {
        if (count++ >= _maxCycles) {
            return 0; // Exceeded timeout, fail.
        }
    }
    // Otherwise fall back to using digitalRead (this seems to be necessary on ESP8266
    // right now, perhaps bugs in direct port access functions?).
#else
    while (digitalRead(_pin) == level) {
        if (count++ >= _maxCycles) {
            return 0; // Exceeded timeout, fail.
        }
    }
#endif

    return count;
}


/*
 * State machine of the non-blocking read.
 */
bool DHTAsync::readAsync() {
    bool status = false;

    switch (dhtState) {
        /* We may begin measuring any time. */
        case DHT_IDLE:
            dhtState = DHT_BEGIN_MEASUREMENT;
            break;

            /* Initiate a sensor read.  The read begins by going to high impedance
               state for 250 ms. */
        case DHT_BEGIN_MEASUREMENT:
            digitalWrite(_pin, HIGH);
            /* Reset 40 bits of received data to zero. */
            data[0] = data[1] = data[2] = data[3] = data[4] = 0;
            dhtTimestamp = millis();
            dhtState = DHT_BEGIN_MEASUREMENT_2;
            break;

            /* After the high impedance state, pull the pin low for 20 ms. */
        case DHT_BEGIN_MEASUREMENT_2:
            /* Wait for 250 ms. */
            if (millis() - dhtTimestamp > 250) {
                pinMode(_pin, OUTPUT);
                digitalWrite(_pin, LOW);
                dhtTimestamp = millis();
                dhtState = DHT_DO_READING;
            }
            break;

        case DHT_DO_READING:
            /* Wait for 20 ms. */
            if (millis() - dhtTimestamp > 20) {
                dhtTimestamp = millis();
                dhtState = DHT_COOLDOWN;
                status = readData();
                if( status != true )
                {
                  // Serial.println( "Reading failed" );
                }
            }
            break;

            /* If it has been less than two seconds since the last time we read
               the sensor, then let the sensor cool down.. */
        case DHT_COOLDOWN:
            if (millis() - dhtTimestamp > COOLDOWN_TIME) {
                dhtState = DHT_IDLE;
            }
            break;

        default:
            break;
    }

    return status;
}


/* Read sensor data.  This is identical to Adafruit's blocking driver. */
bool DHTAsync::readData() {
    uint32_t cycles[80];

      // End the start signal by setting data line high for 20 microseconds.
      digitalWrite(_pin, HIGH);
      delayMicroseconds(20);

      // Now start reading the data line to get the value from the DHT sensor.
      pinMode(_pin, INPUT);
      // Delay a bit to let sensor pull data line low.
      delayMicroseconds(5);

      // First expect a low signal for ~80 microseconds followed by a high signal
      // for ~80 microseconds again.
      if (expectPulse(LOW) == 0) {
          return false;
      }
      if (expectPulse(HIGH) == 0) {
          return false;
      }

      // Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
      // microsecond low pulse followed by a variable length high pulse.  If the
      // high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
      // then it's a 1.  We measure the cycle count of the initial 50us low pulse
      // and use that to compare to the cycle count of the high pulse to determine
      // if the bit is a 0 (high state cycle count < low state cycle count), or a
      // 1 (high state cycle count > low state cycle count). Note that for speed all
      // the pulses are read into a array and then examined in a later step.
      for (int i = 0; i < 80; i += 2) {
          cycles[i] = expectPulse(LOW);
          cycles[i + 1] = expectPulse(HIGH);
      }

    // Inspect pulses and determine which ones are 0 (high state cycle count < low
    // state cycle count), or 1 (high state cycle count > low state cycle count).
    for (int i = 0; i < 40; ++i) {
        uint32_t low_cycles = cycles[2 * i];
        uint32_t high_cycles = cycles[2 * i + 1];
        if ((low_cycles == 0) || (high_cycles == 0)) {
            return false;
        }
        data[i / 8] <<= 1;
        // Now compare the low and high cycle times to see if the bit is a 0 or 1.
        if (high_cycles > low_cycles) {
            // High cycles are greater than 50us low cycle count, must be a 1.
            data[i / 8] |= 1;
        }
        // Else high cycles are less than (or equal to, a weird case) the 50us low
        // cycle count so this must be a zero.  Nothing needs to be changed in the
        // stored data.
    }

    // Check we read 40 bits and that the checksum matches.
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        return true;
    } else {
        return false;
    }
}

/*!
 *  @brief  Converts Celsius to Fahrenheit
 *  @param  c value in Celsius
 *	@return float value in Fahrenheit
 */
float DHTAsync::convertCtoF(float c) { return c * 1.8 + 32; }

/*!
 *  @brief  Converts Fahrenheit to Celsius
 *  @param  f value in Fahrenheit
 *	@return float value in Celsius
 */
float DHTAsync::convertFtoC(float f) { return (f - 32) * 0.55555; }
