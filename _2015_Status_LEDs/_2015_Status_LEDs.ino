// This code written by Mark Crawford for FRC Team 2158 
// Coded during the 2015 build season for Recycle Rush
// Go AusTIN CANs!!!
//
// Some testing has been performed with this code, but I doubt that all possible combinations have been tested. Please use at your own risk.
//
// The Cylon Eye effect was adapted, with many thanks, from code written by EternalCore: https://github.com/EternalCore/NeoPixel_Cylon_Scrolling_Eye
// 
// -------------- General Description --------------
//
// This program is designed to take single characters from a serial input
// and change the color of a section of an LED strip based on the input. An optional LED
// "show" of various kinds can be started upon arduino boot or upon a specific character input. (Intended for use
// when the robot is otherwise idle.) The "show" will end when any character is passed into the serial input.
//
// Use of a WS2812 (aka. NeoPixel) compatible LED string is required (presence of the Adafruit Neopixel library assumed)
//
// The number of sections in the LED display is definable, HOWEVER:
// the number of LEDs in the string is assumed to be evenly divisible by the number of sections specified
// (code behavior is undefined if this requirement is not met)
//
// Having a single white LED between sections is optional. When enabled, relative direction of the string will be 
// indicated by the single white LED lit at the far end of status sections. This may be useful for disambiguation
// as mentioned below. This does reduce the size of each section by one LED.
//
// For usability reasons, care should be taken to avoid ambiguous status color schemes like red-white-red-white-red. It would become more difficult
// to tell which end was which and therefore which section of the display corresponded to which "function" of the robot in such a case.
//
// Be mindful that this code is intended to divide a SINGLE string of WS2812 LEDs into sections. This is probably not what you want if you're working with a
// single string that wraps to different sides of the robot, or where you've got multiple strings daisy chained together all over the robot. The working
// assumption in this code is that if multiple status displays are desired (for instance on multiple sides of the robot), they should all be divided
// in the same way and be showing the same thing. This means that the data input for all of the strings must start at the ARDUINO, not at another string.
// You can build a splitter cable to facilitate this. One partial motivation for this decision was the ability to keep some of the status 
// displays working should one of them take damage during match play. Only one output is provided, which also means that all strings are assumed to have the
// same number of pixels.
// 
// -------------- Details & Settings --------------
// Change things in this section of #define's to customize how this code operates
//
// Each character group below contains 10 characters with each character mapping to a particular color for the associated status display section
// Input chars are defined as follows:
// 0-9 control the first status display section
// a-j control the second status display section
// A-J control the third status display section
// n-w control the fourth status display section
// N-W control the fifth status display section
// Z returns the whole display to the IDLESHOW chosen below
// All other serial inputs have no status display effect, though they will cause the IDLESHOW to cease

// The color to character mappings are as follows and can be changed using the COLORx defines
// Colors are RGB Hex definitions similar to those used in HTML design, they must begin '0x' as seen below
// 0,a,A,n,N White
#define COLOR0 0xFFFFFF
// 1,b,B,o,O Red
#define COLOR1 0xFF0000
// 2,c,C,p,P Green
#define COLOR2 0x00FF00
// 3,d,D,q,Q Blue
#define COLOR3 0x0000FF
// 4,e,E,r,R Cyan
#define COLOR4 0x00FFFF
// 5,f,F,s,S Magenta
#define COLOR5 0xFF00FF
// 6,g,G,t,T Yellow
#define COLOR6 0xFFFF00
// 7,h,H,u,U 50% White (mid brightness White)
#define COLOR7 0x888888
// 8,i,I,v,V 25% White (dim brightness White)
#define COLOR8 0x444444
// 9,j,J,w,W Black
#define COLOR9 0x000000
// Z         Run the IDLESHOW chosen below


// STARTWITHSHOW defines what should happen upon Arduino boot? 
// 0=Nothing (except optional divider display)
// 1=Run chosen IDLESHOW
// 2=Light all LEDs to STARTCOLOR
#define STARTWITHSHOW 0
#define STARTCOLOR 0x00FFFF // Color to have all LEDs lit to upon boot (if set in STARTWITHSHOW) (default: Cyan)

#define BRIGHTNESS 100 // Global brightness percentage (a lower the value is better on your battery but potentially harder to see)
#define SECTIONS 3 // How many status display sections will be shown when not in initial display mode (max acceptable value is 7)

#define DIVIDERS 0 // Have white divider LEDs between status display sections? Only acceptable values: 1=yes, 0=no
#define DIVIDERCOLOR 0x888888 // Color of the divider LEDs (default: mid brightness white)


// Uncomment ONE, and only ONE, of the following two define lines (UART or I2C) to specify the Arduino serial input to be used
// If both are uncommented, no testing has been performed and bugs are more likely, though it may work fine
#define UART
//#define I2C

// Set parameters for the input types here as needed
#define UART_SPEED 115200   // Ignored if UART input isn't used
#define I2C_SLAVE_ADDR 0x10 // Ignored if I2C input isn't used

// Tell me about your WS2812 light strip
#define NPIXEL 10  // How many pixels on the LED strip
#define LED_PIN 6  // What Arduino pin is connected to the data line of the LED strip?
                   // This is always Pin 6 if using the LED header of the REX Robotics RioDuino.

// Choose which Idle time LED show/effect is used
// This can be run in two possible ways: At Arduino boot (see STARTWITHSHOW), or by passing the character 'Z' into the input
// 0=None, all LEDs are off except for (optional) section dividers. Can be used by way of 'Z' char to reset all sections to black.
// 1=Cylon Eye effect going up and down the string
// 2=Cylon Eye effect going only one direction, then wrapping back to the start of the string
// 3=Breathing effect using brightness increase/decrease and a single color
// Solid color
// Zip Up-Down
// Zip Follow
// Rainbow Chase
// Theatre Chase
// Flashing
// Gradient from pre-defined gradients
#define IDLESHOW 1

// Things that control the look of the Cylon Eye effects (ignored if Cylon effects not used)
#define CYLONCOLOR1 0xFFFF00 // Center pixel color
#define CYLONCOLOR2 0x444400 // 2 pixels each side of center (usually just a dimmer version of the center color)
#define CYLONCOLOR3 0x0a0a00 // Background pixels (all pixels not the "eye"
#define CYLONSPEED 80 // Inverse speed, lower = faster (actually a loop delay in millisec)

// Things that control the look of the Breathing effect (ignored if breathing effect not used)
#define BREATHCOLOR 0xFFFF00
#define BREATHSPEED 18 // Inverse speed, lower = faster (actually a loop delay in millisec, note that this delay is executed 5 times between brightness changes)
#define BREATHLOWTIME 1500 // How long (millisec) should the LEDs remain at min brightness? Set to same value as BREATHSPEED to get a sinusoidial pattern.
#define BREATHMAXBRIGHTNESS 128 // How bright should the LEDs get? Set to a value between 4 and 255 (2% to 100% brightness)


// -------------- End of Settings --------------


/////////////////////////////////////////////////////////////////////////////////////
//////////////////                                                 //////////////////
////////////////// Things below here shouldn't need to be modified //////////////////
////////////////// If you make modifications, please share them    //////////////////
//////////////////                                                 //////////////////
/////////////////////////////////////////////////////////////////////////////////////


#ifdef I2C
#include <Wire.h>
#endif

// Effects variables and macros
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define TOGGLE_BIT(var,pos) ((var) ^ (1<<(pos)))
#define SET_BIT(var,pos) ((var) | (1<<(pos)))
#define EFFECTS_COUNT 16
#define MAXSECTIONS 8

boolean dropInitEffect=0;
byte sectionEffect[MAXSECTIONS];
int sectionEffectState[MAXSECTIONS];
unsigned long lastEffectStep[MAXSECTIONS];
uint32_t sectionColor[MAXSECTIONS];

// General execution variables
unsigned int sectionStart[MAXSECTIONS];
unsigned int sectionEnd[MAXSECTIONS];
unsigned int sectionLength=(NPIXEL / SECTIONS);

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NPIXEL, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  sectionStart[MAXSECTIONS-1]=0; // The highest section number always addresses the whole strip
  sectionEnd[MAXSECTIONS-1]=NPIXEL; // The highest section number always addresses the whole strip
#ifdef UART
  Serial.begin(UART_SPEED);
#endif
#ifdef I2C
  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onReceive(receiveI2cEvent); // register event handler
#endif
// Initialize the values defining each section
// MAC: low-priority: figure out if this can be done with constants/pre-compile code
  for(int i=0; i < SECTIONS; i++) {
    sectionStart[i]=sectionLength*i;
    sectionEnd[i]=(sectionLength*(i+1))-DIVIDERS;
    sectionColor[i]=0;
    for(int f=0; f < SECTIONS; f++) {
      sectionEffect[i]=0;
    }
  }
  strip.begin();
  strip.setBrightness(255*(BRIGHTNESS/100.001));
  strip.show();
  doInitEffect();
}

void loop() {
  doEffects();
  strip.show();
  delay(10);
}

void commProtocol(byte incomingByte) {
  static word buffer;
  if(incomingByte == 255) {
    word section = (buffer >> 8) & 0x07;
    word effect = (buffer >> 11) & 0x0F;
    word data = buffer & 0x00FF;
    paramEval(section, effect, data);
  }
  else
    buffer = (buffer << 8) | incomingByte;
}

// This function always processes input for 5 input ranges/display sections
// MAC: low priority: Look at pre-compile directives to only run the code for the defined number of sections
void paramEval(word section, word effect, word color) {
  // If we're doing a static color set, do that and reset the effect value to zero
  // This reduces processing done during doEffects() which is executed frequently and needs to return as quickly as possible
  switch(effect) {
  case 0:
    setSection(section, Wheel(color));
    break;
  case 1: 
    setSection(section, Greyscale(color));
    effect=0;
    break;
  case 4: 
    setSection(section, RYG_Grad(color));
    effect=0;
    break;
  case 5: 
    setSection(section, MW_Grad(color));
    effect=0;
    break;
  }
  setEffect(section, effect, color);
}

void setSection (int section, uint32_t color) {
  if (section == 7) // This means that we need to set all sections to the same color
    for(byte i=0; i<SECTIONS; i++) setSection(i,color);
    
  if (section < SECTIONS) { // Don't do anything if we're told to affect a section that shouldn't exist
    sectionColor[section]=color;
    for(byte i=sectionStart[section]; i<sectionEnd[section]; i++) {
      strip.setPixelColor(i, color);
    }
  }
}

void setEffect(word section, word effect, word data) {
  if(section==7) {
  }
  sectionEffect[section]=effect;
  sectionColor[section]=data;
  lastEffectStep[section]=0;
}

void doEffects() {
  unsigned long now=millis();
  if (now-lastEffectStep[0] < 100) return;

  for(int i=0; i < SECTIONS; i++) {
    if(CHECK_BIT(sectionEffect[i],0)) {
      if(now-lastEffectStep[i] >= 250) {
        toggleBrightness(i);
        lastEffectStep[i]=now;
        Serial.print("FT0 ");
        Serial.println(i);
      }
    }
    else if(CHECK_BIT(sectionEffect[i],1)) {
      if(now-lastEffectStep[i] >= 500) {
        toggleBrightness(i);      
        lastEffectStep[i]=now;
        Serial.print("FT1 ");
        Serial.println(i);
      }
    }
    else if(CHECK_BIT(sectionEffect[i],2)) {
      if(now-lastEffectStep[i] >= 1000) {
        // zipStep(i);
        lastEffectStep[i]=now;
        Serial.print("FT2 ");
        Serial.println(i);
      }
    }
  }
}

void toggleBrightness(int section) {
  if((strip.getPixelColor(sectionStart[section]) & 0xFEFEFE) == (sectionColor[section] & 0xFEFEFE)) {
/*    Serial.print("Got Here, cur color: ");
    Serial.print(strip.getPixelColor(sectionStart[section]),HEX);
    Serial.print(", expected color: ");
    Serial.println(sectionColor[section],HEX); */
    for(int i=sectionStart[section]; i < sectionEnd[section]; i++) 
      strip.setPixelColor(i,0);
  } else {
/*    Serial.print("Got There, cur color: ");
    Serial.print(strip.getPixelColor(sectionStart[section]),HEX);
    Serial.print(", expected color: ");
    Serial.println(sectionColor[section],HEX); */
    for(int i=sectionStart[section]; i < sectionEnd[section]; i++) 
      strip.setPixelColor(i,sectionColor[section]);
  }
}

void initDividers() {
  for(int i=0; i<NPIXEL; i++) {
    strip.setPixelColor(i, 0x000000); //Clears the strand to Black
  }
#if DIVIDERS > 0
  for(uint16_t i=0; i<SECTIONS; i++) {
    int dividerLoc=(sectionLength*(i+1))-1;
    strip.setPixelColor(dividerLoc, DIVIDERCOLOR);
  }
#endif
}
// Input a value 0 to 254 to get an individual color back representing the position within the specified gradient

// Gradient from Red to Green through Yellow (at midpoint)
uint32_t RYG_Grad(byte index) {
  if(index <= 127) {
    return strip.Color(255            , index * 2, 0);
  } else {
    return strip.Color((255-index) * 2, 255      , 0);
  }
}

// Gradient from Magenta to White
uint32_t MW_Grad(byte index) {
    return strip.Color(255, index, 255);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// Function borrowed from AdaFruit NeoPixel library examples
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

// Input a value 0 to 254 to get a color value.
// The colours are a transition black to full white.
uint32_t Greyscale(byte index) {
  if (index > 0) index++;
  return strip.Color(index,index,index);
}

// Input a section and a index position and this produces a color rainbow in that section
uint32_t doRainbow(word section, byte index) {
  for(int i=sectionStart[section]; i < sectionEnd[section]; i++) {
   strip.setPixelColor(i, Wheel(((i * 256 / sectionLength) + index) & 255));
  }
}

////////////////////////////////////////////////
///
/// Below are functions that only get compiled in if certain settings are specified, in some cases 
///
////////////////////////////////////////////////

#ifdef I2C
// function that executes whenever data is received from I2C master
// this function is registered as an event, see setup()
void receiveI2cEvent(int howMany)
{
  if (! dropInitEffect) {
    initDividers();
  }
  dropInitEffect=1;
  while(Wire.available()) // loop through all but the last
  {
    int incomingByte = Wire.read(); // receive byte as a character
    commProtocol(incomingByte); 
  }
}
#endif

#ifdef UART  
void serialEvent() {
  // see if there's incoming serial data:
  if (! dropInitEffect) {
    initDividers();
  }
  dropInitEffect=1;
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    byte incomingByte = Serial.read();
    commProtocol(incomingByte);
  }
}
#endif

// Function to wipe whole string to a specified color
#if STARTWITHSHOW == 2
void doInitColor() {
  for(int i=0; i<NPIXEL; i++) { // Sets whole LED strand to specified color
    strip.setPixelColor(i, STARTCOLOR);
  }
  strip.show();
  while(true) {
    if (dropInitEffect) {
      return;
    }
    delay(50);
  }
}
#endif // STARTWITHSHOW


// Compile in the right doInitDisplay function based on the setting of IDLESHOW as follows
// 0=No effect/show, just init dividers and return
// 1=Cylon Eye effect going up and down the string
// 2=Cylon Eye effect going only one direction, then wrapping back to the start of the string
// 3=Breathing effect using brightness increase/decrease and a single color


// This is the code called when no Idle show/effect is desired
void doInitEffect() {
  setEffect(7,IDLESHOW,0);
#if IDLESHOW == 0
#elif IDLESHOW == 1
#elif IDLESHOW == 2
#elif IDLESHOW == 3  
#endif
}

// End No effect/show


// This is the code for the bi-directional Cylon Eye effect
// The eye runs from one end of the LED string to the other end, the runs in the opposite direction back to the starting end
// Rinse and repeat
// ... Think "Night Rider"
void doEffectCylonEye() {
  if (dropInitEffect) { 
    return; 
  }
  for(int i=0; i<NPIXEL; i++) {
    strip.setPixelColor(i, CYLONCOLOR3); //Clears the dots after the 3rd color
  }
  while(true) {
    for(int i=0; i<NPIXEL; i++) {
      if (dropInitEffect) {
        return;
      }
      strip.setPixelColor(i+2, CYLONCOLOR2); 
      strip.setPixelColor(i+1, CYLONCOLOR2); //Second Dot Color
      strip.setPixelColor(i,   CYLONCOLOR1); //Center Dot Color
      strip.setPixelColor(i-1, CYLONCOLOR2); //Second Dot Color
      strip.setPixelColor(i-2, CYLONCOLOR2); 
      strip.setPixelColor(i-3, CYLONCOLOR3); //Clears the dots to BG color
      strip.show();
      delay(CYLONSPEED);
    }
    for(int i=NPIXEL-1; i>0; i--) {
      if (dropInitEffect) { // When using I2C, the receiveEvent function tells us when to drop out
        return;
      }
      strip.setPixelColor(i-2, CYLONCOLOR2); 
      strip.setPixelColor(i-1, CYLONCOLOR2); //Second Dot Color
      strip.setPixelColor(i,   CYLONCOLOR1); //Center Dot Color
      strip.setPixelColor(i+1, CYLONCOLOR2); //Second Dot Color
      strip.setPixelColor(i+2, CYLONCOLOR2); 
      strip.setPixelColor(i+3, CYLONCOLOR3); //Clears the dots to the BG color
      strip.show();
      delay(CYLONSPEED);
    }
  }
}

// End bi-directional Cylon Eye effect


// This is the code for the looping Cylon Eye effect
// The eye runs from one end of the LED string to the other end
// Once there it keeps going in the same direction, but the eye "wraps" back to the beginning end of the string
// Rinse and repeat
// ... If LED strip(s) are set on a single plane wrapping all the way around the robot, this produces an eye(s) that go all the way around the robot
// ... Keep in mind that there will be as many eyes as you have non-daisy chained LED strips
void doEffectChasingEye() {
  if (dropInitEffect) { 
    return; 
  }
  for(int i=0; i<NPIXEL; i++) {
    strip.setPixelColor(i, CYLONCOLOR3); //Clears the dots after the 3rd color
  }
  while(true) {
    for(int i=3; i<NPIXEL+3; i++) {
      for(int f,j=-2; j<3; j++){
        if ((i+j) >= NPIXEL) {
          f=j-NPIXEL;
        } 
        else {
          f=j;
        }
        if (dropInitEffect) { // When using I2C, the receiveEvent function tells us when to drop out
          return;
        }
        strip.setPixelColor(i+f, CYLONCOLOR2); //Second Dot Color
      }
      if (i >= NPIXEL) {
        strip.setPixelColor(i-NPIXEL, CYLONCOLOR1); //Center Dot Color
      } 
      else {
        strip.setPixelColor(i, CYLONCOLOR1); //Center Dot Color
      }
      strip.setPixelColor(i-3, CYLONCOLOR3); //Clears the dots after the 3rd color
      strip.show();
      delay(CYLONSPEED);
      if (dropInitEffect) { // When using I2C, the receiveEvent function tells us when to drop out
        return;
      }
    }
  }
}

// End looping Cylon Eye effect



// This is the code for the breathing effect
// The LEDs are all set to half brightness and all the same color
// The LEDs are slowly dimmed and brightened 
// ... You get a gentle "breathing/snoring" effect similar to that sometimes seen on an LED in the sleep mode of a computer
void doBreathingEffect() {
  if (dropInitEffect) { 
    return; 
  }
  int brightness=BREATHMAXBRIGHTNESS;
  int changeFactor=-1;

  strip.setBrightness(brightness);
  for(int i=0; i<NPIXEL; i++) {
    if (dropInitEffect) { 
      return; 
    }
    strip.setPixelColor(i, BREATHCOLOR);
  }

  while(true) {
    strip.show();
    if (dropInitEffect) {
      strip.setBrightness(255*(BRIGHTNESS/100.001));
      return;
    }
    delay(BREATHSPEED);
    brightness=brightness+changeFactor;
    if (brightness > BREATHMAXBRIGHTNESS) {
      changeFactor *= -1;
      brightness=brightness+changeFactor;
    }
    if (brightness <= 1) {
      changeFactor *= -1;
      brightness=brightness+changeFactor;
      strip.setBrightness(1);
      strip.show();
      for (int j=0; j < BREATHLOWTIME; j+=20) {
        delay(20);
        if (dropInitEffect) {
          strip.setBrightness(255*(BRIGHTNESS/100.001));
          return;
        }
      }
    }
    strip.setBrightness(brightness);
  }
}

// End breathing effect





