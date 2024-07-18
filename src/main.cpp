#include <Arduino.h>
#include <FastLED.h>
#include <Servo.h>
#include <Adafruit_TiCoServo.h>

#define LED_TYPE    WS2812B  

#define NUM_LEDS 16
#define NUM_LEDS1 8
#define B_NumLeds 4
#define TOP_LEDS 9

#define DATA_PIN 15
#define DATA_PIN1 14
#define DATA_PIN_Breath 7
#define DATA_TOPLEDS 9

#define MAX_FRAMES 10
#define MAX_FRAMES1 22
#define NUM_COLORS 6 // number of blue colors
#define BRIGHTNESS 255

#define SERVO_MIN 500 // 1 ms pulse
#define SERVO_MAX 2350 // 2 ms pulse

Adafruit_TiCoServo servo;

CRGB breathLeds[B_NumLeds]; // breathing LEDs
CRGB topLeds[TOP_LEDS]; // topLeds
CRGB leds[NUM_LEDS]; // circle LEDs
CRGB leds1[NUM_LEDS1]; // barrel LEDs

//For Breathing

static float pulseSpeed = 0.5;  // Larger value gives faster pulse.
uint8_t hueA = 140;  // Start hue at valueMin.
uint8_t satA = 230;  // Start saturation at valueMin.
float valueMin = 120.0;  // Pulse minimum value (Should be less then valueMax).
uint8_t hueB = 171;  // End hue at valueMax.
uint8_t satB = 255;  // End saturation at valueMax.
float valueMax = 255.0;  // Pulse maximum value (Should be larger then valueMin).

//value for comparing prev color states
uint8_t hue = hueA;  
uint8_t sat = satA;  
float val = valueMin;  
uint8_t hueDelta = hueA - hueB;  
static float delta = (valueMax - valueMin) / 2.35040238;  


static float pulseSpeed1 = 0.5;  // Larger value gives faster pulse.
uint8_t hueA1 = 0;  // Start hue at valueMin.
uint8_t satA1 = 0;  // Start saturation at valueMin.
float valueMin1 = 120.0;  // Pulse minimum value (Should be less then valueMax).
uint8_t hueB1 = 0;  // End hue at valueMax.
uint8_t satB1 = 0;  // End saturation at valueMax.
float valueMax1 = 255.0;  // Pulse maximum value (Should be larger then valueMin).

//value for comparing prev color states
uint8_t hue1 = hueA1;  
uint8_t sat1 = satA1;  
float val1 = valueMin1;  
uint8_t hueDelta1 = hueA1 - hueB1;  
static float delta1 = (valueMax1 - valueMin1) / 2.35040238;  




// used for identifying the color direction pattern of the 1st LED in 'leds'
// used in the function 'shiftColors()'
bool ledState = true; // if true: count up, else: count down
bool led1State = true; 

int led1Counter = 0; // used as counter for shiftColors1


// structure of an rgbColor where hex is initialized to 0
struct rgbColor {
  byte red; byte green; byte blue;
};

// colors with the 'rgbColor' structure
// format: colorName = {red, green, blue, hex(optional)}
rgbColor black = {0, 0, 0};
rgbColor darkestBlue = {0, 0, 64};
rgbColor darkerBlue = {0, 0, 128};
rgbColor darkBlue = {0, 0, 160};
rgbColor blue = {0, 0, 255};
rgbColor lightBlue = {0, 128, 255};

// array that contains all variables of rgbColor type
rgbColor rgbColors[NUM_COLORS] = {
  black, darkestBlue, darkerBlue, darkBlue, blue, lightBlue
};

// global array that will serve as the LED's current color reference, meaning
// the index is the LED ID and the value will be its 'rgbColors' ID.
// Assume the values for each LED  will keep changing once the program activates
int ledColorRef[NUM_LEDS] = {
  0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5
};
/* EXAMPLE
e.g. ledColorRef[7] has the value of '3', that means LED 7 will be assigned
the color value in rgbColors[3], which is 'darkBlue'
*/

// LEDS1 color reference
int ledColorRef1[8]={
  0, 0, 0, 0, 0, 0, 0, 0
};


// set the RGB values on each LED in 'leds'
void setLeds() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].red = rgbColors[ledColorRef[i]].red;
    leds[i].green = rgbColors[ledColorRef[i]].green;
    leds[i].blue = rgbColors[ledColorRef[i]].blue;
  }
}
/* EXAMPLE
e.g. assume i = 7:
leds[7].blue = rgbColors[ledColorRef[7]].blue
leds[7].blue = rgbColors[3].blue
leds[7].blue = darkBlue.blue
leds[7].blue = 160
*/

// set the RGB values on each LED in 'leds1'
void setLeds1() {
  for (int i = 0; i < NUM_LEDS1; i++) {
    leds1[i].red = rgbColors[ledColorRef1[i]].red;
    leds1[i].green = rgbColors[ledColorRef1[i]].green;
    leds1[i].blue = rgbColors[ledColorRef1[i]].blue;
  }
}

void setup() {
   pinMode(3,INPUT_PULLUP);
   servo.attach(10, SERVO_MIN, SERVO_MAX);
  Serial.begin(115200);
  delay(2000);
  //while (!Serial) ;
  FastLED.addLeds<LED_TYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, DATA_PIN1, GRB>(leds1, NUM_LEDS1);
  FastLED.addLeds<LED_TYPE, DATA_PIN_Breath, RGB>(breathLeds, B_NumLeds);
  FastLED.addLeds<LED_TYPE, DATA_TOPLEDS, RGB>(topLeds, TOP_LEDS);
  FastLED.setBrightness(80);
  
  //makes sure to clear old LED data
  FastLED.clear();
  FastLED.show();

  // initializing each LED in 'leds1' to have the value of 0, which is 'black'
  setLeds();
  setLeds1();
}
void breath(){
    float dV = ((exp(sin(pulseSpeed * millis()/2000.0*PI)) -0.36787944) * delta);
    val = valueMin + dV;
    hue = map(val, valueMin, valueMax, hueA, hueB);  // Map hue based on current val
    sat = map(val, valueMin, valueMax, satA, satB);  // Map sat based on current val

    for (int i = 0; i < B_NumLeds; i++) {
      breathLeds[i] = CHSV(hue, sat, val);
      breathLeds[i].r = dim8_video(breathLeds[i].r);
      breathLeds[i].g = dim8_video(breathLeds[i].g);
      breathLeds[i].b = dim8_video(breathLeds[i].b);
    }
}
void topBreath(){
    float dV1 = ((exp(sin(pulseSpeed1 * millis()/2000.0*PI)) -0.36787944) * delta1);
    val1 = valueMin1 + dV1;
    hue1 = map(val1, valueMin1, valueMax1, hueA1, hueB1);  // Map hue based on current val
    sat1 = map(val1, valueMin1, valueMax1, satA1, satB1);  // Map sat based on current val

    for (int i = 0; i < TOP_LEDS; i++) {
      topLeds[i] = CHSV(hue1, sat1, val1);
      topLeds[i].r = dim8_video(topLeds[i].r);
      topLeds[i].g = dim8_video(topLeds[i].g);
      topLeds[i].b = dim8_video(topLeds[i].b);
    }
}
void shiftColors() {
  for(int i = NUM_LEDS - 1; i >= 0; i--) {
    if (i == 0) {
      if (ledState) {
        ledColorRef[i] = ledColorRef[i] + 1;
        if (ledColorRef[i] == NUM_COLORS - 1){
          ledState = false;
        }
      } else {
        ledColorRef[i] = ledColorRef[i] - 1;
        if (ledColorRef[i] == 0){
          ledState = true;
        }
      }
      break;
    }
    ledColorRef[i] = ledColorRef[i - 1];
  }
}

void shiftColors1() {
  for(int i = NUM_LEDS1 - 1; i >= 0; i--) {
    if (i == 0) {
      if (led1State) {
        ledColorRef1[i] = ledColorRef1[i] + 1;
        if (ledColorRef1[i] == NUM_COLORS - 1){
          led1State = false;
        }
      } else {
        if (ledColorRef1[i] == 0) {
          if (led1Counter == ((NUM_COLORS - 1) + 6)){
            led1State = true;
            led1Counter = 0;
          } else {
            Serial.print("led1counter:");
            Serial.println(led1Counter);
            led1Counter = led1Counter + 1;
            
          }
          break;
        }

        ledColorRef1[i] = ledColorRef1[i] - 1;
      }
       // Output
      Serial.print("i: ");
      Serial.println(i);
      Serial.print("ledColorRef1[ ");
      for (int j = 0; j < NUM_LEDS1 - 1; j++) {
        Serial.print(ledColorRef1[j]);
        Serial.print(", ");
      }
      Serial.println("]");
      break;
    }
    ledColorRef1[i] = ledColorRef1[i - 1];
     
  }
  
}
void shoot()  {
//tbd tinatamad ako gawin function
}

void loop() {
    topBreath();
    breath();
    EVERY_N_MILLISECONDS(60) { // add line if need specific timing
        shiftColors();  
        setLeds();      
    }
    EVERY_N_MILLISECONDS(90) {
        shiftColors1();
        setLeds1();
    }
   
      if(digitalRead(3)==1){ // checks for button press
        
        servo.write(170); // pushes coins
        Serial.println("open");
      }
      else{
      
        
        servo.write(50); //loads coins
        Serial.println("close");
      }
   
    FastLED.show();
}


