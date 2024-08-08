// Prototype for room sensor controller
// Seeed Studio XIAO SAMD21 version

#ifndef __cplusplus
#error NOT C++
#endif

// for the XIAO SAMD board, the builting LED illuminates when pulled LOW, unlike Arduino Uno
#define LED_ON LOW
#define LED_OFF HIGH


struct SwitchState {
  uint32_t loopCount;
  bool state;
};

uint32_t loopCount = 0;
SwitchState switch1_a;
SwitchState switch1_b;

SwitchState* switch1_current = &switch1_a;
SwitchState* switch1_last = &switch1_b;


void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_A0, INPUT_PULLDOWN);
  pinMode(PIN_A1, INPUT_PULLDOWN);
  pinMode(PIN_A2, INPUT_PULLDOWN);
  pinMode(PIN_A3, INPUT_PULLDOWN);
}

void loop() {
  loopCount++;
  readSwitch(&switch1_current, &switch1_last, PIN_A0);
  dumpSwitchState("last", switch1_last);
  dumpSwitchState("curr", switch1_current);
  Serial.println();

  delay(1000);
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







void readSwitch(SwitchState** pp_current, SwitchState** pp_last, int pin)
{
  SwitchState* temp = *pp_last;
  *pp_last = *pp_current;
  *pp_current = temp;

  (*pp_current)->loopCount = loopCount;
  (*pp_current)->state = digitalRead(pin);
  return;
}





void dumpSwitchState(const char* label, SwitchState* state)
{
  Serial.print(label);
  Serial.print((uint32_t) state);
  Serial.print(" lc: ");
  Serial.print(state->loopCount);
  Serial.print(" st: ");
  Serial.print(state->state);
  Serial.println();
}


// int delay_counter = 1000;

// // the loop function runs over and over again forever
// void loop() {

//   Serial.print(" pin 0 = "); Serial.print(p0);
//   Serial.print(" pin 1 = "); Serial.print(p1);
//   Serial.print(" pin 2 = "); Serial.print(p2);
//   Serial.print(" pin 3 = "); Serial.print(p3);
//   Serial.println();
//   digitalWrite(LED_BUILTIN, LED_ON);
//   delay(50);
//   digitalWrite(LED_BUILTIN, LED_OFF);
//   delay(100);
//   // delay(delay_counter);
//   // delay_counter -= 100;
//   // if (delay_counter < 0)
//   // {
//   //   delay_counter = 1000;
//   // }
// }
