// Prototype for room sensor controller
// edition: Raspberry Pi Pico (v1) using Arduino SDK

#include "PinSampler.hpp"
#include "KeyState.hpp"

const uint8_t CONTROL_KEYS_QTY = 3;


static char dumpBuf[132];

static uint32_t loopCount = 0;
static PinSampler keySampler[CONTROL_KEYS_QTY];
static int keyToPinMap[CONTROL_KEYS_QTY] = { 18, 19, 20};
static KeyState keyStates[CONTROL_KEYS_QTY];


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  for (int i = 0; i < CONTROL_KEYS_QTY; ++i) 
  {
    pinMode(keyToPinMap[i], INPUT_PULLDOWN);
    keyStates[i].SetPressedPinStatus(PinStatus::HIGH);
  }
}



void sampleKeyPins()
{
  for (int i = 0; i < CONTROL_KEYS_QTY; ++i)
  {
    keySampler[i].TakeSample(loopCount, digitalRead(keyToPinMap[i]));
  }
}



void dumpKeyPinSamples()
{
  strcpy(dumpBuf, "key pins: ");
  for (int i = 0; i < CONTROL_KEYS_QTY; ++i)  
  {
    strcat(dumpBuf, keySampler[i].ToString());
    strcat(dumpBuf, " || ");
  }  
  Serial.println(dumpBuf);
}



void updateKeyStates()
{
  for (int i = 0; i < CONTROL_KEYS_QTY; ++i)
  {
    KeyState::KeyPressKind kpk = keyStates[i].Update(keySampler[i]);
    switch (kpk)
    {
      case KeyState::KeyPressKind::ShortPress:
      {
        Serial.println("SHORT PRESS");
        break;
      }
      case KeyState::KeyPressKind::LongPress:
      {
        Serial.println("LONG PRESS");
        break;
      }
      default:
      {
        break;
      }
    }
  }
}



void
dumpKeyStates()
{
  strcpy(dumpBuf, "key states: ");
  for (int i = 0; i < CONTROL_KEYS_QTY; ++i)  
  {
    strcat(dumpBuf, keyStates[i].ToString());
    strcat(dumpBuf, " || ");
  }  
  Serial.println(dumpBuf);
}


void loop() 
{
  loopCount++;
  Serial.print("loopCount "); Serial.print(loopCount); Serial.print(" ");
  int p0 = digitalRead(keyToPinMap[0]);
  int p1 = digitalRead(keyToPinMap[1]);
  int p2 = digitalRead(keyToPinMap[2]);

  //Serial.print(p0); Serial.print(" "); Serial.print(p1); Serial.print(" "); Serial.println(p2);

  sampleKeyPins();
  dumpKeyPinSamples();
  //updateKeyStates();

  delay(200);
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
