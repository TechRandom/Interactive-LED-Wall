#include <Keypad.h>
#include <FastLED.h>

#define SAMPLES 8        // Must be a power of 2
#define LED_PIN     8     // Data pin to LEDS
#define NUM_LEDS    384  
#define BRIGHTNESS  255    // from 0 to 255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB 

#define ROWS 8
#define COLS 8

#define NUM_MODES 6

int values[ROWS][COLS]; // 2D array to keep track of the current color of each tile
int mem_values[ROWS][COLS]; // 2D array to keep track Memory Colors
int brightness[ROWS][COLS]; // 2D array to keep track of the current brightness of each tile

bool escape;

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
//-----------------------------------------------------------------------------------

CRGB leds[NUM_LEDS];            // Create LED Object

int mode = 1;                   // Start on the first mode

CRGBPalette16 currentPalette;   // Used for christmas mode

void setup(){
  Serial.begin(9600);
  delay(1000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  
  buttons.setHoldTime(500); // require a .5 second hold to change modes
  randomSeed(analogRead(0)); //Seed Random
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

  print_menu();
  int location = buttons.getKey();
  mode = buttons.waitForKey();
  switch(mode){
    case 1:
      Serial.println("1");
      rainbowColors();
      break;
    case 2:
      Serial.println("2");
      paint();
      break;
    case 3:
      Serial.println("3");
      Memory();
      break;
    case 4:
      Serial.println("4");
      tic();
      break;
    case 5:
      Serial.println("5");
      filler();
      break;
    case 6:
      Serial.println("5");
      retro();
      break;
  }
//-------------------------------------------------------*/
}

//-----------------------------------------------------------------------------------
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
      values[i][j] = 256;
    }
  }
  FastLED.show();
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void print_menu(){
  clear_display();
  int color = 0;
  for(int i = 1; i <= NUM_MODES; i++){
    light_tile(0, i-1, color, 255);
    color += 32;
  }
  FastLED.show();
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void rainbowColors(){
  clear_display(); 
  int bow = 0;
  int brt = 255;
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
      light_tile(i, j, bow, brt);
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
    if (location)
      brt -= 50;
    if (brt < 100)
      brt = 255;
    for(int i = 0; i < ROWS; i++){
      for(int j = 0; j < COLS; j++){
        int clr = values[i][j] + 1;
        if (clr > 255)
          clr = 0;
        light_tile(i, j, clr, brt);
        if (buttons.getState() == HOLD)
          tap = 0;
      }
    }
    FastLED.show();
  }
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void paint(){
  clear_display();
  bool tap = 1;
  int color = 0;
  while(tap){                                               //Loop until a button is held
    int location = buttons.getKey();
    if (location){                                          //if a button is pressed
      if(values[location/10][(location%10)-1] != 256){
        color = values[location/10][(location%10)-1] + 32;  //Update Color if tile is already colored
        if (color > 256)
          color = 0;
      }  
      else if (color == 256)                                //Loop back to red at end of rainbow
        color = 0;
      light_tile(location/10, (location%10)-1, color, 255); //Light tile color
      values[location/10][(location%10)-1] = color;
      FastLED.show();                                       //Update display
      delay(100);                                           //wait 1/10th of a second
    }
    if(buttons.getState() == HOLD)                          //Exit function if tile held
      tap = 0;
  }
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void Memory(){
  clear_display();                    //Clear display
  for(int i = 2; i < 6; i++){         //Print Verticle lines of box
    light_tile(i, 1, 192, 255);
    light_tile(i, 6, 192, 255);
  }
  for(int i = 1; i < 7; i++){         //Print Horizontal lines of box
    light_tile(1, i, 192, 255);       
    light_tile(6, i, 192, 255);       
  }
  FastLED.show();
  Set_Colors();                       //Set Random Color Locations
  bool tap = 1;
  int color;
  int tile1;
  int tile2;
  int end = 0;
  while(tap){                         //Loop until BUTTON is held
    int location1 = buttons.waitForKey();
    if(buttons.getState() == HOLD)    //Exit if button is held
      tap = 0;
    if (location1){                    //When a button is pushed
      color = mem_values[location1/10][(location1%10)-1];         //Update the color of the clicked tile
      light_tile(location1/10, (location1%10)-1, color, 255);
      tile1 = color;
      FastLED.show();
    }
    int location2 = buttons.waitForKey();
    if(buttons.getState() == HOLD)   //Exit if held
      tap = 0;
    if (location2){                  //When a button is pushed
      color = mem_values[location2/10][(location2%10)-1];         //Update the color of the clicked tile
      light_tile(location2/10, (location2%10)-1, color, 255);
      tile2 = color;
      FastLED.show();
    }
    if(tile1 != tile2 || location1 == location2){              //If tiles are not the same color
        light_tile(location1/10, (location1%10)-1, 256, 255);   //Light the tiles black
        light_tile(location2/10, (location2%10)-1, 256, 255);
    }
    else
      end++;
    delay(1000);
    FastLED.show();
    if(end == 8)
      tap = 0;
  }
}

void Set_Colors(){  //Makes a 4x4 grid of colored pairs in random locations for memory game
  int color = 0;
  for(int i = 0; i < ROWS; i++)
    for(int j = 0; j < COLS; j++)
      mem_values[i][j] = 256;
  for(int j = 0; j < 8; j++){         //Place 8 colors
    for(int i = 0; i< 2; i++){        //place each color twice
      bool tile_not_empty = 1;
      while(tile_not_empty){          //don't place color over another color
        int row = (random()%4)+2;       //Get random tile
        int col = (random()%4)+2;
        if(mem_values[row][col] == 256){  //Check if tile is empty
          mem_values[row][col] = color;   //Place color
          tile_not_empty = 0;         //Exit loop
        }
      }
    }
    color += 32;                      //Increment Color
  }
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void tic(){
  clear_display();
  for(int i = 4; i < ROWS; i+=2){
    for(int j = 0; j < 5; j++){
      light_tile(i, j, 192, 255);
    }
  }
  for(int i = 3; i < ROWS; i++){
    for(int j = 1; j < 4; j+=2){
      light_tile(i, j, 192, 255);
    }
  }
  FastLED.show();
  bool tap = 1;
  bool turn = 0;
  while(tacwinner()){
    int location = buttons.waitForKey();
    if(buttons.getState() == HOLD)
      return;
    else if (location){
      int color = 0;
      if(turn)
        color = 120;
      light_tile(location/10, (location%10)-1, color, 255);
      FastLED.show();
      delay(100);
      turn = !turn;
    }
  }
}

bool tacwinner(){
  for(int i = 3; i < ROWS; i+=2){
    if (values[i][0] == values[i][2] && values[i][0] == values[i][4]){
      if (values[i][0] == 0){
        redwins();
        return 0;
      }
      if (values[i][0] == 120){
        bluewins();
        return 0;
      }
    }
  }
  for(int i = 0; i < 5; i+=2){
    if (values[3][i] == values[5][i] && values[3][i] == values[7][i]){
      if (values[3][i] == 0){
        redwins();
        return 0;
      }
      if (values[3][i] == 120){
        bluewins();
        return 0;
      }
    }
  }
  if (values[3][0] == values[5][2] && values[3][0] == values[7][4]){
    if (values[3][0] == 0){
      redwins();
      return 0;
    }
    if (values[3][0] == 120){
      bluewins();
      return 0;
    }
  }
  if (values[7][0] == values[5][2] && values[7][0] == values[3][4]){
    if (values[7][0] == 0){
      redwins();
      return 0;
    }
    if (values[7][0] == 120){
      bluewins();
      return 0;
    }
  }
  for (int i = 3; i < ROWS; i += 2)
    for (int j = 0; j < 5; j += 2)
      if (values[i][j] == 256)
        return 1;
  nowins();
  return 0;
}

void redwins(){
  for (int i = 0; i < ROWS; i++)
    for (int j = 0; j < COLS; j++)
      light_tile(i, j, 0, 255);
  FastLED.show();
  delay(3000);
  clear_display();
}

void bluewins(){
  for (int i = 0; i < ROWS; i++)
    for (int j = 0; j < COLS; j++)
      light_tile(i, j, 120, 255);
  FastLED.show();
  delay(3000);
  clear_display();
}

void nowins(){
  bool yes = 0;
  for (int i = 0; i < ROWS; i++)
    for (int j = 0; j < COLS; j++){
      int color = 0;
      if(yes)
        color = 120;
      light_tile(i, j, color, 255);
      yes = !yes;
    }
  FastLED.show();
  delay(3000);
  clear_display();
}
//-----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------- 

class Player {
    bool tiles[8][8];
  public:
    int color;
    void setup (int);
    void turn();
    void getSelection();
    void changeColor();
    void addBlocks();
};

void Player::setup (int playerID) {
  for(int i = 0; i < ROWS; i++)
    for(int j = 0; j < COLS; j++)
      tiles[i][j] = 0;
  if (playerID == 1){
    tiles[7][0] = 1;
    color = mem_values[7][0];
  }
  else{
    tiles[1][7] = 1;
    color = mem_values[1][7];
  }
}


void Player::turn(){
  getSelection();
  changeColor();
  addBlocks();
}

void Player::getSelection(){
  //Get Button
  int location = buttons.waitForKey();
  if(location)
    //Get Color at button
    color = mem_values[location/10][(location%10)-1];
}

void Player::changeColor(){
  //Set all values in mem array to value of color if tiles at that index is 1
  for(int i = 0; i < ROWS; i++)
    for(int j = 0; j < COLS; j++){
      if (tiles[i][j] == 1){
        mem_values[i][j] = color;
        light_tile(i, j, color, 150);
      }
    }
  FastLED.show();
}

void Player::addBlocks(){
  for(int i = 0; i < ROWS; i++)
    for(int j = 0; j < COLS; j++)
      //If a tile is unclaimed, matches the new color, and is adjacent to a claimed tile, then add it to claimed tiles
      if (tiles[i][j] == 0 && mem_values[i][j] == color && (tiles[i-1][j]==1 || tiles[i+1][j]==1 || tiles[i][j-1]==1 || tiles[i][j+1]==1))
        tiles[i][j] = 1;
}

void filler(){
  clear_display();
  initializeBoard();
  Player P1;
  Player P2;
  P1.setup(1);
  P2.setup(2);
  escape = 1;
  while(escape){
    P1.turn();
    P2.turn();
    escape = gameOver(P1.color);
  }
}

bool gameOver(int c){
  int last = c, current;
  for(int i = 1; i < ROWS; i++)
    for(int j = 0; j < COLS; j++){
      current = mem_values[i][j];
      if ( current != last)
         return 1;
    }
  return 0;
}

void initializeBoard(){
  for(int i = 0; i < ROWS; i++)
    for(int j = 0; j < COLS; j++)
      mem_values[i][j] = 256;
  for(int i = 1; i < ROWS; i++)
    for(int j = 0; j < COLS; j++)
      while(mem_values[i][j] == 256 || mem_values[i][j] == mem_values[i-1][j] || mem_values[i][j] == mem_values[i][j-1]){
        mem_values[i][j] = (random()%6) * 42;
      }
  for(int i = 0; i < ROWS; i++)
    for(int j = 0; j < COLS; j++)
      light_tile(i, j, mem_values[i][j], 150);
  FastLED.show();
}

//-----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------- 

void retro(){
  clear_display();
  int brt = 255;
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
      if(random()%2 == 0)
        light_tile(i, j, 256, brt);
      else{
        int col = 128 + (32 * (random() % 4));
        light_tile(i, j, col, brt);
      }
    }
  }
  FastLED.show();
  bool tap = 1;
  while(tap){
    int r = random() % ROWS;
    int c = random() % COLS;
    if (values[r][c] == 256){
      int col = 128 + (32 * (random() % 4));
      for (int b = 0; b <= 255; b++){
        light_tile(r, c, col, b);
        FastLED.show();
      }
    }
    else{
      for (int b = 255; b >= 0; b--){
        light_tile(r, c, values[r][c], b);
        FastLED.show();
      }
      light_tile(r, c, 256, 255);
    }
  }
}
