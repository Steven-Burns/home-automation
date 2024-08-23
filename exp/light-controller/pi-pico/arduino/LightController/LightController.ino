// Prototype for LED light controller
// edition: Raspberry Pi Pico RP2040 XIAO board using Arduino SDK
// requires https://github.com/earlephilhower/arduino-pico to be installed in the Arduino IDE board manager.
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


*/

// The rate at which we emit serial data depends on the wire and the board, so parameterize it. 
const unsigned DEBUG_BIT_RATE = 115200;
static char debugLineBuf[132];



#define NUM_LEDS 10
#define DATA_PIN 16s

static CRGB leds[NUM_LEDS];


void setup() {
  Serial.begin(DEBUG_BIT_RATE);
  pinMode(LED_BUILTIN, OUTPUT);
//     pinMode(PIN_LED_G, OUTPUT);
//   pinMode(PIN_LED_B, OUTPUT);

  pinMode(DATA_PIN, OUTPUT);


  FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);  
}



static uint32_t loopCount = 0;

void loop() 
{
  loopCount++;
  Serial.print("loopCount "); Serial.println(loopCount); 

  digitalWrite(LED_BUILTIN, HIGH);
//   digitalWrite(PIN_LED_G, HIGH);
//   digitalWrite(PIN_LED_B, LOW);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Red;
    FastLED.show();
    delay(250);
  }
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    // Now turn the LED off, then pause
    leds[0] = CRGB::Black;
    FastLED.show();
    delay(250);
  }


}
