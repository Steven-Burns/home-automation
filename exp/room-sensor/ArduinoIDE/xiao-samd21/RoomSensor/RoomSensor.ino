// Prototype for room sensor controller
// Seeed Studio XIAO SAMD21 version


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

#define SWITCH_QTY 3
uint switchPinMap[SWITCH_QTY] = { PIN_A0, PIN_A1, PIN_A2};
SwitchState switches_a[SWITCH_QTY];
SwitchState switches_b[SWITCH_QTY];

SwitchState* switches_curr[SWITCH_QTY];
SwitchState* switches_last[SWITCH_QTY];

void initSwitchStateAndPins()
{
  for (int i = 0; i < SWITCH_QTY; ++i)
  {
    switches_curr[i] = &switches_a[i];
    switches_last[i] = &switches_b[i];   
    pinMode(switchPinMap[i], INPUT_PULLDOWN);
  }
}


// read a single switch on the given pin; swapping the current and last state ptrs beforehand
// Thus, calling this func repeatedly results in current pointing to the just-read state and 
// last pointing to the previously read state.
void readSwitch(uint switchnum)
{
  SwitchState** pp_current = &switches_curr[switchnum];
  SwitchState** pp_last = &switches_last[switchnum];
  SwitchState* temp = *pp_last;
  *pp_last = *pp_current;
  *pp_current = temp;

  (*pp_current)->loopCount = loopCount;
  (*pp_current)->state = digitalRead(switchPinMap[switchnum]);
}


void readSwitches()
{
  for (int i = 0; i < SWITCH_QTY; ++i)
  {
    readSwitch(i);
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  initSwitchStateAndPins();
}



void loop() {
  loopCount++;
  Serial.println(loopCount);
  // readSwitches();
  // dumpSwitches();
  delay(1000);
}


const char* statLine = "1 c: v=x lc=yyyy 0xZZZZ l: v=x lc=yyyy 0xZZZZ | 2 c: v=x lc=yyyy 0xZZZZ l: v=x lc=yyyy 0xZZZZ | 3 c: v=x lc=yyyy 0xZZZZ l: v=x lc=yyyy 0xZZZZ ";
const uint valIdx[SWITCH_QTY] = {7, 55, 103};

void dumpSwitches()
{
  for (int i = 0; i < SWITCH_QTY; ++i)
  {
    fmtSwitchDump(i);
  }
  Serial.println(statLine);
}



void fmtSwitchDump(uint i)
{
  SwitchState* ss = switches_curr[i];
  char* bufStart = (char*)statLine + valIdx[i];
  itoa(ss->state, bufStart, 2);
  bufStart += 4;
  itoa(ss->loopCount, bufStart, 16);
  bufStart += 7;
  itoa(((uint32_t)ss & 0xFFFF), bufStart, 16);

  ss = switches_last[i];
  bufStart += 6;
  itoa(ss->state, bufStart, 2);
  bufStart += 4;
  itoa(ss->loopCount, bufStart, 16);
  bufStart += 7;
  itoa(((uint32_t)ss & 0xFFFF), bufStart, 16);
}


