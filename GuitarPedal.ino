#include <Adafruit_NeoPixel.h>
#define PEDAL_BUTTON 0//4 for pedal, 0 for proto
#define PIN 4 //Pin for NeoPixels 
#define NUM_OF_LEDS 4
//Max pot values for 3.3v will be 670
#define MAX_POT 675

//Max pot values for 5.0v will be 1024
//#define MAX_POT 1024
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_OF_LEDS, PIN, NEO_GRB + NEO_KHZ800);

//Drive = A0 (Blue)
//Tone  = A1 (Yellow)
//Level = A2 (White)

//Drive and Level are what brandon uses 
int Drive = A0; //animation step
int Tone = A1;  // Delay speed
int Level = A2; //color select
int max_pot = MAX_POT;
int num_modes = 3; //The amount of modes I have defined in my enum AnimationMode list
int num_rate = 5;  // the amount of rate multiplyer, it will later be used in Rate_Input

unsigned int DriveValue = 0;
unsigned int ToneValue = 0;
unsigned int LevelValue = 0;
unsigned int rate = 5;

unsigned int Button_Press = 0;
//unsigned int state = 0;
//unsigned int old_val = 0;

//I'm using this value for now just to keep the light somewhat dim while I proto this
float myIntensity = .02;  //2% brightness 

enum AnimationMode{
  ON,
  BREATHE,
  BLINK,
  CYCLE,
  SEIZURE,
} mode = ON;

typedef struct {
  int R;
  int G;
  int B;
  int intensity;
} RGB;
  
RGB color, colorPrime;

void setup()
{
  pinMode(PEDAL_BUTTON, INPUT);
  color = {0, 0, 0, 5};
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  Serial.begin(9600);
}


int Clamp(int x) {
  if (x < 0) 
  return 0;
  
  else if (x > 255)
  return 255;
  
  else 
  return x;
}

void loop() 
{
  Color_Input();
  Rate_Input();
  AnimationStep();
  delay(5);
  SetPixels();
}


int AnimationStep(){
  Button_Press = digitalRead(PEDAL_BUTTON);
  DriveValue = analogRead(Drive);  //reads from Drive Pot

  if ((DriveValue >= 0 )                      && (DriveValue <= (max_pot/num_modes)))       { mode  = ON;}
  if ((DriveValue > (max_pot/num_modes))      && (DriveValue <= ((max_pot/num_modes)*2)))   { mode  = BREATHE;}
  if ((DriveValue > ((max_pot/num_modes)*2))  && (DriveValue <= max_pot))                   { mode  = BLINK;}

  //Serial.print("Drive = ");
  //Serial.println(DriveValue);

  if (Button_Press == LOW){
      Seizure();
    }
  else{
  switch (mode){
    
     case ON:
        //Cycle();
        Full_On();
        Serial.println("AnimationStep = ON");
     break;

     case BREATHE:
         Breathe();
         //Serial.println("AnimationStep = BREATHE");
     break;

     case BLINK:
         Blink();
         //Serial.println("AnimationStep = BLINK");
     break;   

    case SEIZURE:
         Seizure();
         //Serial.println("AnimationStep = SEIZURE");
     break;
     
     case CYCLE:
         Cycle();
         //Serial.println("AnimationStep = CYCLE");
     break;
  }
  }
}


void Color_Input()
{
  LevelValue = analogRead(Level);    // reads from Level pot

if (LevelValue <= 255)          // Red to Yellow
  {
    color.R = 255 - LevelValue;           // red goes from on to off
    color.G = LevelValue;                 // green goes from off to on
    color.B = 0;                          // blue is always off
  }

else if (LevelValue <= 511)     // Green to Blue
  {
    color.R = 0;                          // red is always off
    color.G = 255 - (LevelValue - 256);   // green on to off
    color.B = (LevelValue - 256);         // blue off to on
  }

else // color >= 512       // Purple to Red
  {
    color.R = (LevelValue - 512);          // red off to on
    color.G = 0;                           // green is always off
    color.B = 255 - (LevelValue - 512);    // blue on to off
  }
}


void Rate_Input(){
  ToneValue = analogRead(Tone);  //reads from Tone Pot
    if ((ToneValue >= 0 )                    &&  (ToneValue <= (max_pot/num_rate)))    {rate = 2;}
    if ((ToneValue >= (max_pot/num_rate))    &&  (ToneValue <= (max_pot/num_rate)*2))  {rate = 3;}
    if ((ToneValue >= (max_pot/num_rate)*2)  &&  (ToneValue <= (max_pot/num_rate)*3))  {rate = 4;}
    if ((ToneValue >= (max_pot/num_rate)*3)  &&  (ToneValue <= (max_pot/num_rate)*4))  {rate = 5;}
    if ((ToneValue >= (max_pot/num_rate)*4)  &&  (ToneValue <= (max_pot)))             {rate = 6;}
}

void Full_On(){
  colorPrime = color;
  colorPrime.intensity = 3;
}


void Breathe(){
static enum {
   inhale,
   exhale 
  }respiration = exhale;

  static int brightness = 50;

 if (brightness > color.intensity){
      brightness = color.intensity;
      respiration = exhale;
 }
  
  if (respiration == exhale){
    if (brightness == 0){
      respiration = inhale;
      brightness++;
    }
    else 
      brightness--;
  }

  else{
    if (brightness == color.intensity){
      respiration = exhale;
      brightness--;
    }
    else brightness++;
  }

  // use the value of colorPrime for output 
  colorPrime = color;
  colorPrime.intensity = brightness;
  Serial.println(brightness);
  delay(20*rate); //100
}

void Blink(){
  static enum{
    on,
    off,
    middle
  }type = on;

  static int brightness = 0;

  if (type == on){
    delay(100*rate); //500
    brightness = 3;
    type = middle;
  }
  
  else if (type == middle){
    type = off;
  }
  
  else {
    delay(100*rate); //500
    brightness = 0;
    type = on;
  }
  
  colorPrime = color;
  colorPrime.intensity = brightness;
}

void Cycle(){

  for (int x = 0; x < 768; x++){
      DriveValue = x;
      colorPrime = color;
      colorPrime.intensity = 3;
      delay(4);
  }
}

void Seizure(){
  static enum{
    red,
    green,
    blue,
    yellow,
    purple,
    aqua,
    white
  }interval = red;
 
  if (interval == red)
  {
    color.R = 255,
    color.G = color.B = 0;
    interval = green;
  }

  else if (interval == green)
  {
    color.R = color.B = 0,
    color.G = 255;
    interval = blue;
  }

  else if (interval == blue)
  {
    color.R = color.G = 0,
    color.B = 255;
    interval = yellow;
  }

    else if (interval == yellow)
  {
    color.R = color.G = 255,
    color.B = 0;
    interval = purple;
  }

    else if (interval == purple)
  {
    color.R = color.B = 255,
    color.G = 0;
    interval = aqua;
  }

    else if (interval == aqua)
  {
    color.B = color.G = 255,
    color.R = 0;
    interval = white;
  }

    else if (interval == white)
  {
    color.R = color.G = color.B = 255;
    interval = red;
  }
 
  delay(rate * 17);  //85
  colorPrime = color;
  colorPrime.intensity = 2;
}

void SetPixels(){
  //Serial.println(colorPrime.intensity);
  SetLED(strip.Color(colorPrime.R * colorPrime.intensity*myIntensity, colorPrime.G * colorPrime.intensity*myIntensity, colorPrime.B * colorPrime.intensity*myIntensity), 0); 
}

void SetLED(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}




