//Author:@vincentmakes
//Instructions:https://hackaday.io/project/164043-the-infinity-usb-knob
//GNU Licence

#include "HID-Project.h"
#include "Adafruit_NeoPixel.h"


#define PIN  4//to change to DI
#define NUMPIXELS      16 //adjust number of pixels in the ring
int MaxPixelID=NUMPIXELS-1; //because first pixel is 0
volatile byte mybrightness = 36; //max brightness when turning the knob
volatile byte lowbrightness = 2; //initial brightness
int MaxMode=5; //defines how many modes we want. The code further down is pre-written for 5 modes (0 to 4) but this integer allows to trim off while not having to delete functions.
 
Adafruit_NeoPixel pixels=Adafruit_NeoPixel(NUMPIXELS,PIN,NEO_GRB+NEO_KHZ800);


int encoderPinA = 1; 
int encoderPinB = 2;
int buttonPin = 3; //Pin C (push)

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

  //call updateEncoder() when any high/low changed seen on Pin A or B
  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoder, CHANGE);


  Consumer.begin(); //initialize HID
  Keyboard.begin(); // initialize keyboard

  //Initialize Neopixel
  pixels.setBrightness(lowbrightness);
  pixels.begin();
  //lit all pixels with lower brightness 
  for( int i = 0; i<NUMPIXELS; i++){
        pixels.setPixelColor(i, Red[Mode],Green[Mode],Blue[Mode]);
    }   
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
        pixels.setPixelColor(i, Red[Mode],Green[Mode],Blue[Mode]);
    }   
    //pixels.setPixelColor(0, Red[Mode],Green[Mode],Blue[Mode]); //display first led with new color and lower brightness
    pixels.show();

 }

  delay(50); 

}//end loop



//2 types Led Animation. The pixels in the neoring 16 are ordered in counterclokwise way while the 8 one is counterclockwise. The code below is for counterclockwise setup.
void AnimationStop(signed int KnobValue, int Red, int Green, int Blue) //Animation in one direction only with pixel 0 as starting point |----->
{
   
    for(int i=0;i<NUMPIXELS;i++) { //turn all off
       pixels.setPixelColor(i, 0,0,0); 

    }
    
  if(KnobValue<0)
  {
    encoderValue=0;  
  }
  
  if(KnobValue>0) 
  { 
    int MinValue=min(abs(MaxPixelID-KnobValue),MaxPixelID);
    
  for(int i=MaxPixelID;i>MinValue;i--) { 
       pixels.setPixelColor(i, Red,Green,Blue);   
    }
   }

    
    pixels.setPixelColor(0, Red,Green,Blue); //pixel 0 always on
    pixels.setBrightness(mybrightness);
    pixels.show();

}


void AnimationContinuous(signed int KnobValue, int Red, int Green, int Blue) //Animation bidirectional centered on pixel 0 <------|------>
{

   for(int i=0;i<NUMPIXELS;i++) { //turn all off
       pixels.setPixelColor(i, 0,0,0); 
    }
    
  if(KnobValue<0) {
    int MaxValue=abs(KnobValue);
    
    for(int i=0;i<MaxValue;i++){ 
      
       pixels.setPixelColor(i, Red,Green,Blue);

    }
  }
  
  if(KnobValue>0) 
  { 
    int MinValue=min(abs(MaxPixelID-KnobValue),MaxPixelID);
    
  for(int i=MaxPixelID;i>MinValue;i--) { 
       pixels.setPixelColor(i, Red,Green,Blue); 
        
    }
   }

    
    pixels.setPixelColor(0, Red,Green,Blue); //pixel 0 always on
    pixels.setBrightness(mybrightness);
    pixels.show();


    
}

//Here's all the shortcuts pre-defined. CW=clockwise; CCW=counterclockwise. Do NOT put any delay() in those functions as it is incompatible with the interrupt

void ShortcutActionCW0(){//Undo

//Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_CTRL);
Keyboard.write('z');
Keyboard.releaseAll();//release
}

void ShortcutActionCCW0(){//Redo

//Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_CTRL);
Keyboard.press(KEY_LEFT_SHIFT);//SHIFT+CMD+z
Keyboard.write('z');
Keyboard.releaseAll();//release
}

void ShortcutActionCW1(){
    Consumer.write(MEDIA_VOLUME_UP);

}

void ShortcutActionCCW1(){
    Consumer.write(MEDIA_VOLUME_DOWN);

}

void ShortcutActionCW2(){
Keyboard.press(KEY_LEFT_GUI);
//Keyboard.write('w');
Keyboard.releaseAll();//release
}

void ShortcutActionCCW2(){
Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_SHIFT);//SHIFT+CMD+z
//Keyboard.write('t');
Keyboard.releaseAll();//release
}

void ShortcutActionCW3(){
Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_ALT);//SHIFT+CMD+z
Keyboard.press(KEY_RIGHT_ARROW);
Keyboard.releaseAll();//release
}

void ShortcutActionCCW3(){
Keyboard.begin();
Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_ALT);//SHIFT+CMD+z
Keyboard.press(KEY_LEFT_ARROW);
Keyboard.releaseAll();//release
}

void ShortcutActionCW4(){

}

void ShortcutActionCCW4(){

}

///////End of Shortcuts definition

void updateEncoder(){ //this function is called on interrupt

  OldValue=readEncoderValue();
  int MSB = digitalRead(encoderPinA); //MSB = most significant bit
  int LSB = digitalRead(encoderPinB); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++; 
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

 NewValue=readEncoderValue();

  if(NewValue!=OldValue)
  {
 Serial.println(NewValue);
  }
  
  if(NewValue>OldValue) //Clockwise turn
  {
    Serial.println("Forward");

     if(Mode==0)
    {
        ShortcutActionCW0();
        AnimationStop(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }
     if(Mode==1)
    {
        ShortcutActionCW1();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }
      if(Mode==2)
    {
        ShortcutActionCW2();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }
     if(Mode==3)
    {
        ShortcutActionCW3();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }
      if(Mode==4)
    {
        ShortcutActionCW4();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }


  }

   if(NewValue<OldValue) //AntiClockwise turn
  {
    Serial.println("Backward");
    if(Mode==0)
    {
        ShortcutActionCCW0();
        AnimationStop(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    if(Mode==1)
    {
        ShortcutActionCCW1();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    if(Mode==2)
    {
        ShortcutActionCCW2();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    if(Mode==3)
    {
        ShortcutActionCCW3();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

      if(Mode==4)
    {
        ShortcutActionCCW4();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    
  }
  
  lastEncoded = encoded; //store this value for next time
  
}
