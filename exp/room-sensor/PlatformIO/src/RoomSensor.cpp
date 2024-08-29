// Prototype for room sensor device
// edition: XAIO RP2040 using Arduino SDK

#include <Arduino.h>
#include <Wire.h>
#include "KeyState.hpp"
#include "DHT11Async.hpp"
#include "GroveLightSensor12.hpp"
#include "GroveMiniPIRMotionSensor.hpp"
#include "Sampler.hpp"
#include "SensorEvent.hpp"
#include "Display.hpp"
#include "Reboot.hpp"

// The rate at which we emit serial data depends on the wire and the board, so parameterize it.
const unsigned DEBUG_BIT_RATE = 115200;
static char debugLineBuf[132];

static uint32_t loopCount = 0;
static uint32_t loop1Count = 0;

static struct
{
  uint keypresses = 0;
  uint temperatureChanges = 0;
  uint motionChanges = 0;
  uint lightChanges = 0;
  uint pingCount = 0;
  uint heartbeatCount = 0;
} stats;

const uint8_t CONTROL_KEYS_QTY = 3;
// The pins used by each key switch
static uint8_t keyToPinMap[CONTROL_KEYS_QTY] = {D8, D9, D10};
static KeyState keyStates[CONTROL_KEYS_QTY];

// The frequency to take a temp sensor reading
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

void doDisplaySetup()
{
  Wire.begin(); // Needed to get I2C working well
  Display::Setup();
}

void clearBuiltinLEDs()
{
  // on the XAIO RP2040, you turn the builtin LEDs off by setting them high, weird.
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(PIN_LED_B, HIGH);
  digitalWrite(PIN_LED_G, HIGH);
}

// non-blocking heartbeat
void heartbeatCPU0()
{
  static int lastLEDLevel = LOW;
  static uint32_t millisSinceLastCall = 0;
  uint32_t now = millis();
  if (now - millisSinceLastCall > 1500)
  {
    lastLEDLevel = (lastLEDLevel == HIGH) ? LOW : HIGH;
    digitalWrite(PIN_LED_B, lastLEDLevel);
    millisSinceLastCall = now;
  }
}

// non-blocking heartbeat for the second CPU
void hearbeatCPU1()
{
  static int lastLEDLevel = LOW;
  static uint32_t millisSinceLastCall = 0;
  uint32_t now = millis();
  if (now - millisSinceLastCall > 500)
  {
    lastLEDLevel = (lastLEDLevel == HIGH) ? LOW : HIGH;
    digitalWrite(PIN_LED_G, lastLEDLevel);
    millisSinceLastCall = now;
  }
}

void setup()
{
  delay(1000);
  Serial.begin(DEBUG_BIT_RATE);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);

  clearBuiltinLEDs();
  doDisplaySetup();
  doKeyStatesSetup();
  doTemperatureAndHumiditySetup();
  doMotionSensorSetup();
  doLightSensorSetup();
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
      DHTSample s = {.temperature = t, .humidity = h};
      dhtSampler.TakeSample(millis(), s);
      return true;
    }
  }

  return false;
}

void updateDisplay()
{
  static uint32_t lastUpdateTime = millis();
  uint32_t now = millis();
  if (now - lastUpdateTime > 250)
  {
    Display::StartUpdate();
    sprintf(
        debugLineBuf,
        "Arcadia House       %d.%d.%d.%d",
        // TODO put ip address here
        255, 255, 255, 255);
    Display::WriteString(0, 0, debugLineBuf);

    sprintf(
        debugLineBuf,
        "K %02d M %02d L %02d T %03d P %04d H %03d",
        stats.keypresses, stats.motionChanges, stats.lightChanges, stats.temperatureChanges,
        stats.pingCount, stats.heartbeatCount);
    Display::WriteString(0, 1, debugLineBuf);

    Display::WriteString(0, 2, "More fun than a box of rocks on a");
    Display::WriteString(0, 3, "cloudy day.");

    Display::EndUpdate();

    // TODO: replace this with actual IP ping counter
    stats.pingCount = loopCount;

    lastUpdateTime = now;
  }
}

void loop()
{
  loopCount++;
  heartbeatCPU0();
  // Serial.print("loopCount "); Serial.print(loopCount); Serial.print(" ");

  if (measureTemperatureAndHumidity() && dhtSampler.HasSampleChanged())
  {
    TemperatureEvent te(
        dhtSensor.convertCtoF(dhtSampler.Current()->value.temperature),
        dhtSampler.Current()->value.humidity);
    stats.temperatureChanges++;
    Serial.println(te.ToString());
  }

  lightSampler.TakeSample(millis(), lightSensor.ReadLevel());
  if (lightSampler.HasSampleChanged())
  {
    LightLevelEvent le(lightSampler.Current()->value);
    stats.lightChanges++;
    Serial.println(le.ToString());
  }

  motionSampler.TakeSample(millis(), motionSensor.IsMotionDetected());
  if (motionSampler.HasSampleChanged())
  {
    MotionEvent me(motionSampler.Current()->value);
    stats.motionChanges++;
    Serial.println(me.ToString());
  }

  // TODO
  // read pico internal temperature
  // comms over rs485 and Cat6A cables 100'
  // heartbeat: every M units of time, send a full passive sensor update with current states

  updateDisplay();
  delay(15);

  // TODO: this wasn't working reliably. And 10K iterations is too soon.  Might be more reliable
  // to wire a pin to the reset pin, if there is one.
  // if (loopCount > 9999)
  // {
  //   reboot();
  // }
}

void sampleKeyPins()
{
  for (int i = 0; i < CONTROL_KEYS_QTY; ++i)
  {
    KeyState::KeyPressKind k = keyStates[i].Update(digitalRead(keyToPinMap[i]));
    if (k != KeyState::KeyPressKind::None)
    {
      KeyPressEvent kpe(i, k);
      stats.keypresses++;
      Serial.println(kpe.ToString());
    }
  }
}

void loop1()
{
  loop1Count++;
  hearbeatCPU1();

  // sampling the keyboard pins as fast as possible on CPU1 works well. Fewer missed keystrokes.
  sampleKeyPins();
}