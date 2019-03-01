#include "HID-Project.h"
#include "Adafruit_NeoPixel.h"


#define PIN  10//to change to DI
#define NUMPIXELS      8 //adjust number of pixels in the ring
volatile byte mybrightness = 16; //max brightness when turning the knob
volatile byte lowbrightness = 8; //initial brightness
int MaxMode=5; //defines how many modes we want. The code further down is pre-written for 5 modes (0 to 4) but this integer allows to trim off while not having to delete functions.
 
Adafruit_NeoPixel pixels=Adafruit_NeoPixel(NUMPIXELS,PIN,NEO_GRB+NEO_KHZ800);


int encoderPinA = 2; 
int encoderPinB = 3;
int buttonPin = 1; //Pin C (push)

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;
long OldValue = 0;
long NewValue =0;

int lastMSB = 0;
int lastLSB = 0;
int Mode =0, Red[5] = { 0, 0, 255, 255,255 }, Green[5]={145,255,119,0,255}, Blue[5]={255,30,0,239,0} ; //default mode on startup + define colors for each mode
/*Mode 0->Blue (0,145,255) ; 1-> Green (0,255,30) ; 2->Orange ; 3-> Purple ; 5-> Yellow*/

long readEncoderValue(void){
    return encoderValue/4;
}

boolean isButtonPushDown(void){
  if(!digitalRead(buttonPin)){
    delay(50);
    if(!digitalRead(buttonPin))
      return true;
  }
  return false;
}

void setup() {
  Serial.begin (9600);

  pinMode(encoderPinA, INPUT); 
  pinMode(encoderPinB, INPUT);
  pinMode(buttonPin, INPUT);

  digitalWrite(encoderPinA, HIGH); //turn pullup resistor on
  digitalWrite(encoderPinB, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);


  Consumer.begin(); //initialize HID

  //Initialize Neopixel
  pixels.setBrightness(lowbrightness);
  pixels.begin();
  pixels.setPixelColor(0, Red[0],Green[0],Blue[0]); //lit first pixel with lower brightness 
  pixels.show();
} //end setup

void loop(){ 


  if(isButtonPushDown()){
    Serial.println("you push button down!!!");
    Mode++;
    encoderValue = 0;
    if(Mode==MaxMode){Mode=0;} //cycle through Modes
    delay(100);
    pixels.setBrightness(lowbrightness);
    for( int i = 0; i<NUMPIXELS; i++){
        pixels.setPixelColor(i, 0,0,0);
    }   
    pixels.setPixelColor(0, Red[Mode],Green[Mode],Blue[Mode]); //display first led with new color and lower brightness
    pixels.show();

 }

  delay(50); 

}//end loop

//4 types Led Animation 


void AnimationStopFW(signed int KnobValue, int Red, int Green, int Blue) //Animation when turning the knob clockwise (one direction only)
{
    for(int i=0;i<KnobValue;i++) { 
      pixels.setBrightness(mybrightness);
       pixels.setPixelColor(i, Red,Green,Blue); 
    }
    for(int i=KnobValue;i<NUMPIXELS;i++) { 
      
       pixels.setPixelColor(i, 0,0,0, 0); 
    }
    pixels.setPixelColor(0, Red,Green,Blue);
    pixels.show();

}

void AnimationStopBW(signed int KnobValue, int Red, int Green, int Blue) //Animation when turning the knob anti-clockwise (one direction only)
{
  
  if(KnobValue<0) {KnobValue=0;encoderValue=0;}
  
  for(int i=KnobValue;i<NUMPIXELS;i++) { 
   
       pixels.setPixelColor(i, 0,0,0,0); 
    }
    pixels.setPixelColor(0, Red,Green,Blue);
    pixels.show();

}


void AnimationContinuousFW(signed int KnobValue, int Red, int Green, int Blue) //Animation when turning the knob clockwise (bidirectional)
{

     if(KnobValue<0) {
    int MinValue=NUMPIXELS+KnobValue;
    for(int i=0;i<MinValue;i++) {
      
       pixels.setPixelColor(i, 0,0,0); 
    }
    pixels.setPixelColor(0, Red,Green,Blue);
  }
  

      if(KnobValue>=0)
  {
    for(int i=0;i<KnobValue;i++) { 
       pixels.setPixelColor(i, Red,Green,Blue); 
    }
    for(int i=KnobValue;i<NUMPIXELS;i++) { 
       pixels.setPixelColor(i, 0,0,0); 
    }
  }
  pixels.setPixelColor(0, Red,Green,Blue);
    pixels.show();
    
}

void AnimationContinuousBW(signed int KnobValue, int Red, int Green, int Blue) //Animation when turning the knob anti-clockwise (bidirectional)
{

  if(KnobValue<0) {// If -2 LEDs #8+#7 on
    int MinValue=NUMPIXELS+KnobValue;
    //Serial.println(MinValue);
    for(int i=NUMPIXELS;i>MinValue;i--){ 
      pixels.setBrightness(mybrightness);
       pixels.setPixelColor(i, Red,Green,Blue);
       Serial.println(i); 
    }
  }
  
  if(KnobValue>=0)
  {
  for(int i=KnobValue;i<NUMPIXELS;i++) { 
       pixels.setPixelColor(i, 0,0,0); 
    }
    pixels.setPixelColor(0, Red,Green,Blue);
  }

    pixels.show();
    
}

//Here's all the shortcuts pre-defined. FW=forward or clockwise; BW=backward or counterclockwise

void ShortcutActionFW0(){//Undo
Keyboard.begin();
Keyboard.press(KEY_LEFT_GUI);
Keyboard.write('z');
delay(100);
Keyboard.releaseAll();//release
}

void ShortcutActionBW0(){//Redo
Keyboard.begin();
Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_SHIFT);//SHIFT+CMD+z
Keyboard.write('z');
delay(100);
Keyboard.releaseAll();//release
}

void ShortcutActionFW1(){
    Consumer.write(MEDIA_VOLUME_UP);
    delay(100);
    Keyboard.releaseAll();//release
}

void ShortcutActionBW1(){
    Consumer.write(MEDIA_VOLUME_DOWN);
    delay(100);
    Keyboard.releaseAll();//release
}

void ShortcutActionFW2(){
Keyboard.begin();
Keyboard.press(KEY_LEFT_GUI);
Keyboard.write('w');
delay(100);
Keyboard.releaseAll();//release
}

void ShortcutActionBW2(){
Keyboard.begin();
Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_SHIFT);//SHIFT+CMD+z
Keyboard.write('t');
delay(100);
Keyboard.releaseAll();//release
}

void ShortcutActionFW3(){
Keyboard.begin();
Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_ALT);//SHIFT+CMD+z
Keyboard.press(KEY_RIGHT_ARROW);
delay(100);
Keyboard.releaseAll();//release
}

void ShortcutActionBW3(){
Keyboard.begin();
Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_ALT);//SHIFT+CMD+z
Keyboard.press(KEY_LEFT_ARROW);
delay(100);
Keyboard.releaseAll();//release
}

void ShortcutActionFW4(){

}

void ShortcutActionBW4(){

}

///////End of Shortcuts definition

void updateEncoder(){ //this function is called on interrupt

  OldValue=readEncoderValue();
  int MSB = digitalRead(encoderPinA); //MSB = most significant bit
  int LSB = digitalRead(encoderPinB); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue --;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue ++;

 NewValue=readEncoderValue();

  if(NewValue!=OldValue)
  {
 Serial.println(NewValue);
  }
  
  if(NewValue>OldValue) //Clockwise turn
  {
    Serial.println("Forward");

    switch(Mode) {
      case 0 :
        ShortcutActionFW0();
        AnimationStopFW(NewValue, Red[Mode], Green[Mode], Blue[Mode]);

      case 1:
        ShortcutActionFW1();
        AnimationContinuousFW(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    

      case 2:
        ShortcutActionFW2();
        AnimationContinuousFW(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    
      case 3:
        ShortcutActionFW3();
        AnimationContinuousFW(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    

      case 4:
        ShortcutActionFW4();
        AnimationContinuousFW(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }


  }

   if(NewValue<OldValue) //AntiClockwise turn
  {
    Serial.println("Backward");
    if(Mode==0)
    {
        ShortcutActionBW0();
        //if(NewValue<0){encoderValue=0;}
        AnimationStopBW(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    if(Mode==1)
    {
        ShortcutActionBW1();
        AnimationContinuousBW(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    if(Mode==2)
    {
        ShortcutActionBW2();
        AnimationContinuousBW(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    if(Mode==3)
    {
        ShortcutActionBW3();
        AnimationContinuousBW(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

      if(Mode==4)
    {
        ShortcutActionBW4();
        AnimationContinuousBW(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    
  }
  
  lastEncoded = encoded; //store this value for next time
  
}
