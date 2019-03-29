/*****
 * 
 * Gully + bar RGB
 * Gully = APA102 (SPI: 10,11,12,13)
 * Bar = 5050 via P9813 (pin 0,1)
 * 
 * Gully leds contains two stripts placed behind echother as on strip, 
 * however the code is treathing them as two strips.
 * 
 * Follow https://home-assistant.io/components/light.mqtt_json/ over USB
 * 
 */

#include <FastLED.h>  

//Pin Layout
#define ANALOG_CLOCK  7
#define ANALOG_DATA   9
#define GULLY_SS    10
#define GULLY_MOSI  11  //Data output
#define GULLY_MISO  12
#define GULLY_SCK   13  //Clock

//Serial configuration
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        
char serialCommand[numChars] = {0};
char serialValue[numChars] = {0};
boolean newSerialData = false;
boolean transition = true;

//Fastled generic configuration
#define UPDATES_PER_SECOND 100
uint8_t brightness = 130;         // 0-255
uint8_t effect = 2;               // 0-10 0:current effect; 1:new effect
uint8_t effect_speed = 50;        // 0-255
int color_left[3] = {200,0,0};
int color_right[3] = {200,0,0};
CRGB tmpClrR;                     // Temp holder for a color
CRGB tmpClrL;                     // Temp holder for a color

//Fastled GULLY - APA102
#define GULLY_LED_TYPE    APA102
#define GULLY_COLOR_ORDER BGR
#define GULLY_NUM_LEDS    83 //per strip
#define GULLY_DATA_RATE   24
CRGB gully_leds[GULLY_NUM_LEDS*2];
uint32_t tmpReverse[GULLY_NUM_LEDS];  // Temp holder for a color
CRGBSet gully_leds_all(gully_leds, GULLY_NUM_LEDS);
CRGBSet gully_leds_left(gully_leds_all(0,GULLY_NUM_LEDS - 1));
CRGBSet gully_leds_right(gully_leds_all(GULLY_NUM_LEDS,GULLY_NUM_LEDS*2));

//Fastled configuration ANALOG leds
#define ANALOG_LED_TYPE     P9813
#define ANALOG_COLOR_ORDER  RGB
#define BAR_NUM_LEDS        1 
#define WALL_NUM_LEDS       5 
#define ANALOG_NUM_LEDS     BAR_NUM_LEDS + WALL_NUM_LEDS
CRGB analog_leds[ANALOG_NUM_LEDS];
CRGBSet analog_leds_all(analog_leds, ANALOG_NUM_LEDS);
CRGBSet bar_leds(analog_leds_all(0,0));
CRGBSet wall_leds(analog_leds_all(1,5));

//Timers
unsigned long testMillis = 0;    
int test_step = 0;

void setup() {
  //Setup serial
  Serial.begin(9600);

  //Setup Fastled
  FastLED.addLeds<GULLY_LED_TYPE,GULLY_MOSI,GULLY_SCK,GULLY_COLOR_ORDER,DATA_RATE_MHZ(GULLY_DATA_RATE)>(gully_leds,GULLY_NUM_LEDS*2);
  FastLED.addLeds<ANALOG_LED_TYPE,ANALOG_DATA,ANALOG_CLOCK,ANALOG_COLOR_ORDER>(analog_leds,ANALOG_NUM_LEDS);
  FastLED.setBrightness(brightness);
}

void loop() {
  //Get serial Data
  recvWithStartEndMarkers();

  //Set Light
  runProgramm();
}
