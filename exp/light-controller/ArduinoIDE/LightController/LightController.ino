// Prototype for LED light controller
// edition: Raspberry Pi Pico v1 Genuine using Arduino SDK
// Using the genuine pico as it supports 5V pinouts, which for the LED thingies I have
// works better than the 3.3V XIAO RP2040 board.
// requires the Pico MBed board support be installed in the Arduino IDE board manager -- FastLED 
// won't work on the MBED-less arduino board setup.
// Requires FastLED library


#include <FastLED.h>

// The job here is to take "lighting commands" and ... make the lights connected to the device illuminate accordingly.

// What would a command vocabulary look like?
/*

A selector of "effects over time" options, and the parameters for each of those.

transition to <current state> to RGBW over N ms, repeat M times.  M = -1 == repeat forever. Step function is an enum: linear, logarhythmic, random(?), accelerated (with an acceleration factor)
  Immediate OFF would look like 0,0,0,0 over 0ms, no repeat (M = 0)

flicker
glow in out repeat
glow up repeat
flash

The upstream automator that is authoring the effect command is responsible for 
- determining the scene
- determining the addressed pixels
- scaling the intensity based on factors such as time-of-day

Defer this until you have a better understanding of what FastLED has to offer, which 
includes concepts like pallettes, global brightness and such.

*/

// The rate at which we emit serial data depends on the wire and the board, so parameterize it. 
const unsigned DEBUG_BIT_RATE = 115200;
static char debugLineBuf[132];



#define NUM_LEDS 10
#define DATA_PIN 16

static CRGB leds[NUM_LEDS];


void setup() {
  Serial.begin(DEBUG_BIT_RATE);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);  
}




void incrementalOn()
{
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::White;
    FastLED.show();
    delay(100);
  }
}

void incrementalOff()
{
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    // Now turn the LED off, then pause
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(100);
  }
}

// Implement a simple command interpreter

// null if nothing available, else the contents of the serial buffer.


const byte numChars = 65;
char receivedChars[numChars];   // an array to store the received data

static boolean newData = false;


void recvWithEndMarker() {
    static byte ndx = 0;
    static char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else if (-1 == rc) 
        {
          // something is fishy, as there is nothing in the read buffer after available claimed there was...
          Serial.println("fishiness!");
          continue;
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
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
    digitalWrite(LED_BUILTIN, lastLEDLevel);
    millisSinceLastCall = now;
  }
}


int firstParam(char* command)
{
  int result = 0;
  char* remainder = command;
  char* token = strtok_r(command, " ", &remainder);

  if (token) 
  {
    // look for the next (the second) token -- this is the first parameter to the command
    token = strtok_r(remainder, " ", &remainder);
    if (token)
    {
      result = atoi(token);
    }
  }

  return result;
}



void serviceCommandLoop()
{
  recvWithEndMarker();
  if (!newData) 
  {
    return;
  }

  Serial.print("Command received: ");
  Serial.println(receivedChars);
  newData = false;

  if (strcmp(receivedChars, "alloff") == 0)
  {
    Serial.println("Doing all off");
    incrementalOff();
  }
  else if (strcmp(receivedChars, "allon") == 0)
  {
    Serial.println("Doing all on");
    incrementalOn();
  }
  else if (strncmp(receivedChars, "on", 2) == 0)
  {
    int led = firstParam(receivedChars);
    Serial.print("turning on led "); Serial.println(led);
    leds[led] = CRGB::White;

  }
  else if (strncmp(receivedChars, "off", 3) == 0)
  {
    int led = firstParam(receivedChars);
    Serial.print("turning off led "); Serial.println(led);
    leds[led] = CRGB::Black;
  }
  else
  {
    Serial.println("no comprendo.");
    return;
  }
  FastLED.show();
}


static uint32_t loopCount = 0;
void loop() 
{
  loopCount++;
 // Serial.print("loopCount "); Serial.println(loopCount); 

  toggleBuiltinLED();
  serviceCommandLoop();
}
