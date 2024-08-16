// Prototype for room sensor device
// edition: Raspberry Pi Pico (v1) using Arduino SDK


#include "KeyState.hpp"
#include "DHT11Async.hpp"
#include "GroveLightSensor12.hpp"
#include "GroveMiniPIRMotionSensor.hpp"
#include "Sampler.hpp"


// The rate at which we emit serial data depends on the wire and the board, so parameterize it. 
const unsigned DEBUG_BIT_RATE = 115200;
static char debugLineBuf[132];

const uint8_t CONTROL_KEYS_QTY = 3;
// The pins used by each key switch 
static uint8_t keyToPinMap[CONTROL_KEYS_QTY] = { 2, 3, 4 };
static KeyState keyStates[CONTROL_KEYS_QTY];

static uint8_t DHT_SENSOR_PIN = 20; 
DHTAsync dhtSensor(DHT_SENSOR_PIN);

struct DHTSample 
{
  float temperature;
  float humidity;

  bool operator!=(DHTSample rhs) { return (temperature != rhs.temperature || humidity != rhs.humidity); }
};

Sampler<DHTSample> dhtSampler;

static uint8_t LIGHT_SENSOR_PIN = A0;
GroveLightSensor lightSensor(LIGHT_SENSOR_PIN);
Sampler<GroveLightSensor::LightLevel> lightSampler;

static uint8_t MOTION_SENSOR_PIN = 18;
GroveMotionSensor motionSensor(MOTION_SENSOR_PIN);
Sampler<bool> motionSampler;


// The frequency to take a sensor reading
static ulong MEASUREMENT_INTERVAL_MS = 5000ul;


void doKeyStatesSetup()
{
  for (int i = 0; i < CONTROL_KEYS_QTY; ++i) 
  {
    pinMode(keyToPinMap[i], INPUT_PULLDOWN);
  }
}

void doTemperatureAndHumiditySetup()
{

}

void doLightSetup()
{

}

void doMotionSensorSetup()
{
  pinMode(MOTION_SENSOR_PIN, INPUT);
}



void setup() {
  Serial.begin(DEBUG_BIT_RATE);
  pinMode(LED_BUILTIN, OUTPUT);

  doKeyStatesSetup();
  doTemperatureAndHumiditySetup();
  doLightSetup();
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



/*
 * Poll for a measurement, keeping the state machine alive.  Returns
 * true if a measurement is available.
 */
static bool measureTemperatureAndHumidity() 
{
    static ulong measurement_timestamp = millis();
    static float t = NAN;
    static float h = NAN;

    if (millis() - measurement_timestamp > MEASUREMENT_INTERVAL_MS) 
    {
      if (dhtSensor.measure(&t, &h)) 
      {
        measurement_timestamp = millis();
        DHTSample s = { .temperature = t, .humidity = h };
        dhtSampler.TakeSample(millis(), s);
        return true;
      }
    }

    return false;
}



// static bool measureLight(GroveLightSensor::LightLevel& level)
// {
//   static ulong measurement_timestamp = millis();

//   if (millis() - measurement_timestamp > MEASUREMENT_INTERVAL_MS) {
//     level = lightSensor.ReadLevel();
//     measurement_timestamp = millis();
//     return true;
//   }
//   return false;
// }



// static bool measureMotion(bool& motionDetected)
// {
//   static ulong measurement_timestamp = millis();

//   if (millis() - measurement_timestamp > MEASUREMENT_INTERVAL_MS) {
//     motionDetected = motionSensor.IsMotionDetected();
//     measurement_timestamp = millis();
//     return true;
//   }
//   return false;
// }



static uint32_t loopCount = 0;

void loop() 
{
  loopCount++;
  // Serial.print("loopCount "); Serial.print(loopCount); Serial.print(" ");

  sampleKeyPins();
 
  // Push the temp and humidity state machine forward. If the function returns
  // true, then a measurement is available.
  if (measureTemperatureAndHumidity() && dhtSampler.HasSampleChanged()) {
      Serial.print("T = ");
      Serial.print(dhtSensor.convertCtoF(dhtSampler.Current()->value.temperature), 1);
      Serial.print(" F  H = ");
      Serial.print(dhtSampler.Current()->value.humidity, 1);
      Serial.println("%");
  }

  lightSampler.TakeSample(millis(), lightSensor.ReadLevel());
  if (lightSampler.HasSampleChanged())  
  {
    Serial.print("light "); Serial.println(lightSampler.Current()->value);
  }

  motionSampler.TakeSample(millis(), motionSensor.IsMotionDetected());
  if (motionSampler.HasSampleChanged())
  {
    Serial.print("motion "); Serial.println(motionSampler.Current()->value);
  }

  delay(15);
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
