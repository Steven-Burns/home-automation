// Prototype for room sensor device
// edition: Raspberry Pi Pico (v1) using Arduino SDK


#include <Arduino.h>
#include "KeyState.hpp"
#include "DHT11Async.hpp"
#include "GroveLightSensor12.hpp"
#include "GroveMiniPIRMotionSensor.hpp"
#include "Sampler.hpp"
#include "SensorEvent.hpp"



// The rate at which we emit serial data depends on the wire and the board, so parameterize it. 
const unsigned DEBUG_BIT_RATE = 115200;
static char debugLineBuf[132];

void clearBuiltinLED()
{
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(PIN_LED_B, HIGH);
  digitalWrite(PIN_LED_G, HIGH);
}

// non-blocking heartbeat
void toggleBuiltinLED()
{
  static int lastLEDLevel = LOW;
  static uint32_t millisSinceLastCall = 0;
  uint32_t now = millis();
  if (now - millisSinceLastCall > 1000) 
  {
    lastLEDLevel =(lastLEDLevel == HIGH) ? LOW : HIGH;
    //digitalWrite(LED_BUILTIN, lastLEDLevel);
    digitalWrite(PIN_LED_B, lastLEDLevel);
    //digitalWrite(PIN_LED_G, lastLEDLevel);
    millisSinceLastCall = now;
  }
}



/*
QUESTION to self

does the sensor interpret the meaning of the keys, or just the actions taken with them?  e.g.
LIGHT_ON / LIGHT_OFF or "BUTTON 1 SINGLE CLICK"?

probably the latter, as interpretation may need to consider information not available to the sensor, like
time of day, sunset, day of year, etc.

*/


const uint8_t CONTROL_KEYS_QTY = 3;
// The pins used by each key switch 
static uint8_t keyToPinMap[CONTROL_KEYS_QTY] = { D8, D9, D10};
static KeyState keyStates[CONTROL_KEYS_QTY];

// The frequency to take a sensor reading
static ulong TEMPERATURE_MEASUREMENT_INTERVAL_MS = 5000ul;
static uint8_t DHT_SENSOR_PIN = D2; 
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

static uint8_t MOTION_SENSOR_PIN = D1;
GroveMotionSensor motionSensor(MOTION_SENSOR_PIN);
Sampler<bool> motionSampler;



void doKeyStatesSetup()
{
  for (int i = 0; i < CONTROL_KEYS_QTY; ++i) 
  {
    pinMode(keyToPinMap[i], INPUT_PULLDOWN);
  }
}

void doTemperatureAndHumiditySetup()
{
  pinMode(DHT_SENSOR_PIN, INPUT);
}

void doLightSensorSetup()
{
  pinMode(LIGHT_SENSOR_PIN, INPUT);

}

void doMotionSensorSetup()
{
  pinMode(MOTION_SENSOR_PIN, INPUT);
}



void setup() {
  delay(1000);
  Serial.begin(DEBUG_BIT_RATE);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);

  clearBuiltinLED();

  doKeyStatesSetup();
  doTemperatureAndHumiditySetup();
  doMotionSensorSetup();
  doLightSensorSetup();
}



void sampleKeyPins()
{
  for (int i = 0; i < CONTROL_KEYS_QTY; ++i)
  {
    KeyState::KeyPressKind k = keyStates[i].Update(digitalRead(keyToPinMap[i]));
    if (k != KeyState::KeyPressKind::None)
    {
      KeyPressEvent kpe(i, k);
      Serial.println(kpe.ToString());
    }
  }
}



// Push the temp and humidity state machine forward. If the function returns
// true, then a measurement is available.
static bool measureTemperatureAndHumidity() 
{
    static ulong measurement_timestamp = millis();
    static float t = NAN;
    static float h = NAN;

    if (millis() - measurement_timestamp > TEMPERATURE_MEASUREMENT_INTERVAL_MS) 
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



static uint32_t loopCount = 0;

void loop() 
{
  loopCount++;
  toggleBuiltinLED();
  // Serial.print("loopCount "); Serial.print(loopCount); Serial.print(" ");

  sampleKeyPins();
 
  if (measureTemperatureAndHumidity() && dhtSampler.HasSampleChanged()) 
  {
    TemperatureEvent te(
        dhtSensor.convertCtoF(dhtSampler.Current()->value.temperature),
        dhtSampler.Current()->value.humidity);
    Serial.println(te.ToString());
  }

  lightSampler.TakeSample(millis(), lightSensor.ReadLevel());
  if (lightSampler.HasSampleChanged())  
  {
    LightLevelEvent le(lightSampler.Current()->value);
    Serial.println(le.ToString());
  }

  motionSampler.TakeSample(millis(), motionSensor.IsMotionDetected());
  if (motionSampler.HasSampleChanged())
  {
    MotionEvent me(motionSampler.Current()->value);
    Serial.println(me.ToString());
  }

  // TODO
  // read pico internal temperature
  // comms over rs485 and Cat6A cables 100'
  // heartbeat: every M units of time, send a full passive sensor update with current states


  delay(15);
}
