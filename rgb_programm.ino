/*
 * Programms
 * 0: Turn off (Fadeing supported)
 * 1: Solid color (Fadeing supported)
 * 
 */
DEFINE_GRADIENT_PALETTE( fire_gp ) {
    0,   1,  1,  0,
   76,  32,  5,  0,
  146, 192, 24,  0,
  197, 220,105,  5,
  240, 252,255, 31,
  250, 252,255,111,
  255, 255,255,255};

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {  rainbow, rainbowWithGlitter, confetti, sinelon, juggle }; //left bpm out
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint16_t scale = 30;
static uint16_t x;
static uint16_t y;
static uint16_t z;
uint8_t noise[2][GULLY_NUM_LEDS];
static CRGBPalette16 gCurrentPalette = fire_gp;
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void runProgramm(){
  if(effect == 0){
    //0: Turn off
    if(!transition){
      //Turn all off if it was running any programm and transition is off
      FastLED.clear();
    }else{
      //Fade to off
      gully_leds_all.fadeToBlackBy(40);
      analog_leds_all.fadeToBlackBy(40);
    }
  }else if(effect == 1){
    //1: Set solid color
    if(!transition){
      fill_solid(gully_leds_all, GULLY_NUM_LEDS*2, CRGB(color_left)); 
      fill_solid(analog_leds_all, ANALOG_NUM_LEDS, CRGB(color_left)); 
    }else{
      fadeTowardColor( gully_leds_all, GULLY_NUM_LEDS*2, CRGB(color_left), 40);
      fadeTowardColor( analog_leds_all, ANALOG_NUM_LEDS, CRGB(color_left), 40);
    }
  }else if (effect == 2){
    // Music demo real
    // Call the current pattern function once, updating the 'leds' array
    // gPatterns[gCurrentPatternNumber]();
    
    // do some periodic updates
    // EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    // EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  }else if(effect == 3){
    //pre-Movie

  }else if(effect == 4){
    //Movie

  }else if(effect == 5){
    //game

  }else if(effect == 6){
    //test run
    testRoutine();
  }

  //invert
  for (int i=0, j = GULLY_NUM_LEDS-1; i< GULLY_NUM_LEDS/2; i++, j--){
    CRGB temp = gully_leds_left[i];
    gully_leds_left[i] = gully_leds_left[j];
    gully_leds_left[j] = temp;
  }

  //Set leds
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

// Helper function that blends one uint8_t toward another by a given amount
void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount) {
  if ( cur == target) return;
  if ( cur < target ) {
    uint8_t delta = target - cur;
    delta = scale8_video( delta, amount);
    cur += delta;
  } else {
    uint8_t delta = cur - target;
    delta = scale8_video( delta, amount);
    cur -= delta;
  }
}

// Blend one CRGB color toward another CRGB color by a given amount.
// Blending is linear, and done in the RGB color space.
// This function modifies 'cur' in place.
CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount) {
  nblendU8TowardU8( cur.red,   target.red,   amount);
  nblendU8TowardU8( cur.green, target.green, amount);
  nblendU8TowardU8( cur.blue,  target.blue,  amount);
  return cur;
}

// Fade an entire array of CRGBs toward a given background color by a given amount
// This function modifies the pixel array in place.
void fadeTowardColor( CRGB* L, uint16_t N, const CRGB& bgColor, uint8_t fadeAmount) {
  for ( uint16_t i = 0; i < N; i++) {
    fadeTowardColor( L[i], bgColor, fadeAmount);
  }
}


void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( gully_leds_left, GULLY_NUM_LEDS, gHue, 7);
  fill_rainbow( wall_leds, WALL_NUM_LEDS, gHue, 20);
  
  //mirror to right strip
  gully_leds_right = gully_leds_left;

  //Add to bar led
  bar_leds[0] = gully_leds_left[0];
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    gully_leds_left[ random16(GULLY_NUM_LEDS) ] += CRGB::White;
  }
  if( random8() < chanceOfGlitter) {
    gully_leds_right[ random16(GULLY_NUM_LEDS) ] += CRGB::White;
  }
  if( random8() < chanceOfGlitter) {
    wall_leds[ random16(WALL_NUM_LEDS) ] += CRGB::White;
  }
}

void addValueWave(int bpm, int valMin, int valMax) 
{
  for(byte i = 0; i < WALL_NUM_LEDS;i++){
    wall_leds[i] = CHSV(i*5, 255, beatsin8(bpm, valMin, valMax));
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( gully_leds_left, GULLY_NUM_LEDS, 10);
  int pos = random16(GULLY_NUM_LEDS);
  gully_leds_left[pos] += CHSV( gHue + random8(64), 200, 255);
  
  //mirror to right strip
  gully_leds_right = gully_leds_left;

  //Add to bar led
  fill_solid(bar_leds, ANALOG_NUM_LEDS, gully_leds_left[0]); 
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( gully_leds_left, GULLY_NUM_LEDS, 20);
  int pos = beatsin16(13,0,GULLY_NUM_LEDS);
  gully_leds_left[pos] += CHSV( gHue, 255, 192);

  //mirror to right strip
  gully_leds_right = gully_leds_left;

  //Add to bar led
  bar_leds[0] = gully_leds_left[0];
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( gully_leds_left, GULLY_NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    gully_leds_left[beatsin16(i+7,0,GULLY_NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  
  //mirror to right strip
  gully_leds_right = gully_leds_left;

  //Add to bar led
  bar_leds[0] = gully_leds_left[0];
}

void testRoutine() {
 if((testMillis + 1000) > millis()){
    fill_solid(gully_leds_all, ANALOG_NUM_LEDS, CRGB::Red); 
    fill_solid(analog_leds_all, GULLY_NUM_LEDS*2, CRGB::Red); 
  }else if((testMillis + 2000) > millis()){
    fill_solid(gully_leds_all, ANALOG_NUM_LEDS, CRGB::Green); 
    fill_solid(analog_leds_all, GULLY_NUM_LEDS*2, CRGB::Green); 
  }else if((testMillis + 3000) > millis()){
    fill_solid(gully_leds_all, ANALOG_NUM_LEDS, CRGB::Blue); 
    fill_solid(analog_leds_all, GULLY_NUM_LEDS*2, CRGB::Blue); 
  }else if((testMillis + 4000) > millis()){
    FastLED.clear(); 
    fill_solid(gully_leds_left, GULLY_NUM_LEDS, CRGB::White); 
  }else if((testMillis + 5000) > millis()){
    FastLED.clear();
    fill_solid(gully_leds_right, GULLY_NUM_LEDS, CRGB::White); 
  }else if((testMillis + 6000) > millis()){
    FastLED.clear();
    bar_leds[0] = CRGB::White;
  }else if((testMillis + 7000) > millis()){
    FastLED.clear();
    wall_leds[0] = CRGB::White;
  }else if((testMillis + 8000) > millis()){
    FastLED.clear();
    wall_leds[1] = CRGB::White;
  }else if((testMillis + 9000) > millis()){
    FastLED.clear();
    wall_leds[2] = CRGB::White;
  }else if((testMillis + 10000) > millis()){
    FastLED.clear();
    wall_leds[3] = CRGB::White;
  }else if((testMillis + 11000) > millis()){
    FastLED.clear();
    wall_leds[4] = CRGB::White;
  }else{
    testMillis = millis();
  }
}

