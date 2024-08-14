
// Prototype for room sensor controller
// edition: Raspberry Pi Pico (v1) using Arduino SDK


#include "KeyState.hpp"
#include "DHT11Async.hpp"


// The rate at which we emit serial data depends on the wire and the board, so parameterize it. 
const unsigned DEBUG_BIT_RATE = 115200;
const uint8_t CONTROL_KEYS_QTY = 3;



static char debugLineBuf[132];
static uint32_t loopCount = 0;

// The pins used by each key switch 
static int keyToPinMap[CONTROL_KEYS_QTY] = { 2, 3, 4 };
static KeyState keyStates[CONTROL_KEYS_QTY];




DHTAsync dhtSensor(20);


/*
 * Poll for a measurement, keeping the state machine alive.  Returns
 * true if a measurement is available.
 */
static bool measureTemperatureAndHumidity(float *temperature, float *humidity) {
    static unsigned long measurement_timestamp = millis();

    /* Measure once every four seconds. */
    if (millis() - measurement_timestamp > 4000ul) {
        if (dhtSensor.measure(temperature, humidity)) {
            measurement_timestamp = millis();
            return (true);
        }
    }

    return (false);
}



void setup() {
  Serial.begin(DEBUG_BIT_RATE);
  pinMode(LED_BUILTIN, OUTPUT);

  for (int i = 0; i < CONTROL_KEYS_QTY; ++i) 
  {
    pinMode(keyToPinMap[i], INPUT_PULLDOWN);
  }
}



void sampleKeyPins()
{
  for (int i = 0; i < CONTROL_KEYS_QTY; ++i)
  {
    KeyState::KeyPressKind k = keyStates[i].Update(digitalRead(keyToPinMap[i]));
    if (k != KeyState::KeyPressKind::None)
    {
      Serial.println(k);
    }
  }
}



static float temperature;
static float humidity;

void loop() 
{
  loopCount++;
  // Serial.print("loopCount "); Serial.print(loopCount); Serial.print(" ");

  sampleKeyPins();
 

  /* Measure temperature and humidity.  If the functions returns
      true, then a measurement is available. */

  if (measureTemperatureAndHumidity(&temperature, &humidity)) {
      Serial.print("T = ");
      Serial.print(dhtSensor.convertCtoF(temperature), 1);
      Serial.print(" F  H = ");
      Serial.print(humidity, 1);
      Serial.println("%");
  }




  delay(10);
}







// passive sensors
// read motion sensor
// read light sensor
// read temp sensor
// read humidity sensor

// active sensors
// read switches, interpret keyboard state

// switch events are more "urgent" than the others.
// also, we want to send a heartbeat
// and, we don't want to flood the upstream recipient with "non-events"
// strategy:
// every 5 units of time, read the active sensors, send event if a state changes
// every N units of time, read the passive sensors, send event if a state changes
// every M units of time, send a full passive sensor heartbeat with current state





// ControlKey interpretation code; independent of the board that it might be running on.

// A control key represents a user interface button that can fire the following 'events'
//  short press
//  long press
//  double press
// This code samples pin states and timings and determines from a pin's transitions which of the 
// above interpretations apply. 








//pin sampling - generic to any pin, offers "change" detection

//keypress interpretation of pins connected to switches
