#include <Keypad.h>
#include <FastLED.h>

#define LED_PIN     8     // Data pin to LEDS
#define NUM_LEDS    384  
#define BRIGHTNESS  255    // from 0 to 255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB 

#define ROWS 8
#define COLS 8

#define NUM_MODES 3

int values[ROWS][COLS]; // 2D array to keep track of the current color of each tile
int brightness[ROWS][COLS]; // 2D array to keep track of the current brightness of each tile

//-----------------------------Button Set Up-----------------------------------------
const byte rows = 8;
const byte cols = 8;
char keys[rows][cols] = {
  {1,2,3,4,5,6,7,8},
  {11,12,13,14,15,16,17,18},
  {21,22,23,24,25,26,27,28},
  {31,32,33,34,35,36,37,38},
  {41,42,43,44,45,46,47,48},
  {51,52,53,54,55,56,57,58},
  {61,62,63,64,65,66,67,68},
  {71,72,73,74,75,76,77,78}
};
byte rowPins[rows] = {10,9,2,3,4,5,6,7}; //connect to the row pinouts of the keypad
byte colPins[cols] = {38,40,42,44,46,48,50,52}; //connect to the column pinouts of the keypad
Keypad buttons = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );
//--------------------------=--------------------------------------------------------

CRGB leds[NUM_LEDS];            // Create LED Object

int mode = 1;                   // Start on the first mode

CRGBPalette16 currentPalette;   // Used for christmas mode

void setup(){
  Serial.begin(9600);
  delay(1000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  
  buttons.setHoldTime(1000); // require a 1 second hold to change modes

  clear_display(); //Make sure the display is blank
}

void loop(){
  /* --------Light tiles one at a time (testing)-----------
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
      light_tile(i, j, 0, 255);
      FastLED.show();
      delay(10);
      light_tile(i, j, 256, 0);
    }
  }
  -------------------------------------------------------*/

/*  //---------Light tile when pressed (testing)------------
  int location = buttons.getKey();
  if (location){
    light_tile(location/10, (location%10)-1, 0, 255);
    FastLED.show();
    delay(100);
  }
  // else 
    //clear_display();
 // -------------------------------------------------------*/

 //--------------- Actual Code ---------------------------

  int location = buttons.getKey();
  if (location)
    mode++;
  if(mode > NUM_MODES)
    mode = 1;
  switch(mode){
    case 1:
      rainbowColors();
      mode++;
      break;
    case 2:
      paint();
      mode++;
      break;
    case 3:
      christmas();
      mode++;
      break;
  }
//-------------------------------------------------------*/
}

void light_tile(int row, int col, int color, int bright){
  if (color == 256)
    bright = 0;
  for(int i=23; i >= 21; i--){
    leds[i - 3*row + 48*col]= CHSV( color, 255, bright);
  }
  for(int i=24; i <= 26; i++){
    leds[i + 3*row + 48*col]= CHSV( color, 255, bright);
  }
  values[row][col] = color;
  brightness[row][col] = bright;
}

void clear_display(){
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
      light_tile(i, j, 256, 0);
    }
  }
  FastLED.show();
}

void rainbowColors(){
  int bow = 0;
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
      light_tile(i, j, bow, 255);
      bow += 32;
      if(bow > 255)
         bow = 0;
    }
    bow += 32;
    if(bow > 255)
      bow = 0;
  }
  FastLED.show();
  bool tap = 1;
  while(tap){
    int location = buttons.getKey();
    for(int i = 0; i < ROWS; i++){
      for(int j = 0; j < COLS; j++){
        int clr = values[i][j] + 1;
        if (clr > 255)
          clr = 0;
        light_tile(i, j, clr, 255);
        if (location)
          tap = 0;
      }
    }
    FastLED.show();
  }
}

void paint(){
  clear_display();
  bool tap = 1;
  while(tap){
    int location = buttons.getKey();
    if (location){
       int color = values[location/10][(location%10)-1] + 32;
       if (color > 256)
          color = 0;
      light_tile(location/10, (location%10)-1, color, 255);
      FastLED.show();
      delay(100);
    }
    if(buttons.getState() == HOLD)
      tap = 0;
  }
}

void christmas(){
  bool tap = 1;
  int location = buttons.getKey();
  CRGB red = CHSV( HUE_RED, 255, 255);
  CRGB green  = CHSV( HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;
  CRGB white  = CRGB::White;
  
  currentPalette = CRGBPalette16(green,  green,  black,  black,
                                 red, red, white,  white,
                                 green,  green,  black,  black,
                                 red, red, white,  white);
  while(tap){
    static uint8_t startIndex = 0;
    startIndex++;
    for( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = ColorFromPalette( currentPalette, startIndex, 255, LINEARBLEND);
      startIndex += 3;
    }
    FastLED.show();
    FastLED.delay(10);
    if(location)
      tap = 0;
  }
}
