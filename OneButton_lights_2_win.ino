//updated 7mar

#include "HID-Project.h"
#include "Adafruit_NeoPixel.h"


#define PIN  4//to change to DI
#define NUMPIXELS      16 //adjust number of pixels in the ring
volatile byte mybrightness = 24; //max brightness when turning the knob
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

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoder, CHANGE);


  Consumer.begin(); //initialize HID
  Keyboard.begin();

  //Initialize Neopixel
  pixels.setBrightness(lowbrightness);
  pixels.begin();
  //pixels.setPixelColor(0, Red[0],Green[0],Blue[0]); //lit first pixel with lower brightness 
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

//4 types Led Animation 


void AnimationStop(signed int KnobValue, int Red, int Green, int Blue) //Animation when turning the knob clockwise (one direction only)
{


    for(int i=0;i<NUMPIXELS;i++) { 
       pixels.setPixelColor(i, 0,0,0); 

    }
    
  if(KnobValue<0)
  {
    encoderValue=0;  
  }
  if(KnobValue>=0) //if 1 then 16
  { 
    int MinValue=min(NUMPIXELS-KnobValue,NUMPIXELS);
    
  for(int i=NUMPIXELS;i>MinValue;i--) { 
       pixels.setPixelColor(i, Red,Green,Blue); 
        
    }
      }
pixels.setPixelColor(0, Red,Green,Blue);
pixels.setBrightness(mybrightness);
    pixels.show();

}


void AnimationContinuous(signed int KnobValue, int Red, int Green, int Blue) //Animation when turning the knob clockwise (bidirectional)
{
   for(int i=0;i<NUMPIXELS;i++) { 
       pixels.setPixelColor(i, 0,0,0); 

    }
    
  if(KnobValue<0) {
    int MaxValue=abs(KnobValue);
    
    for(int i=0;i<MaxValue;i++){ 
      
       pixels.setPixelColor(i, Red,Green,Blue);

    }
  }
  
  if(KnobValue>=0) 
  { 
    int MinValue=min(NUMPIXELS-KnobValue,NUMPIXELS);
    
  for(int i=NUMPIXELS;i>MinValue;i--) { 
       pixels.setPixelColor(i, Red,Green,Blue); 
        
    }

  }
pixels.setPixelColor(0, Red,Green,Blue);
pixels.setBrightness(mybrightness);
    pixels.show();

  
  
}

//Here's all the shortcuts pre-defined. FW=forward or clockwise; BW=backward or counterclockwise

void ShortcutActionFW0(){//Undo

//Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_CTRL);
Keyboard.write('z');
Keyboard.releaseAll();//release
}

void ShortcutActionBW0(){//Redo

//Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_CTRL);
//Keyboard.press(KEY_LEFT_SHIFT);//SHIFT+CMD+z
Keyboard.write('y');
Keyboard.releaseAll();//release
}

void ShortcutActionFW1(){
    Consumer.write(MEDIA_VOLUME_UP);

}

void ShortcutActionBW1(){
    Consumer.write(MEDIA_VOLUME_DOWN);

}

void ShortcutActionFW2(){
Keyboard.press(KEY_LEFT_GUI);
//Keyboard.write('w');
Keyboard.releaseAll();//release
}

void ShortcutActionBW2(){
Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_SHIFT);//SHIFT+CMD+z
//Keyboard.write('t');
Keyboard.releaseAll();//release
}

void ShortcutActionFW3(){
Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_ALT);//SHIFT+CMD+z
Keyboard.press(KEY_RIGHT_ARROW);
Keyboard.releaseAll();//release
}

void ShortcutActionBW3(){
Keyboard.begin();
Keyboard.press(KEY_LEFT_GUI);
Keyboard.press(KEY_LEFT_ALT);//SHIFT+CMD+z
Keyboard.press(KEY_LEFT_ARROW);

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

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

 NewValue=readEncoderValue();

  if(NewValue!=OldValue)
  {
 //Serial.println(NewValue);
  }
  
  if(NewValue>OldValue) //Clockwise turn
  {
    Serial.println("Forward");

     if(Mode==0)
    {
        ShortcutActionFW0();
        AnimationStop(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }
     if(Mode==1)
    {
     
        ShortcutActionFW1();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }
 if(Mode==2)
    {
      
        ShortcutActionFW2();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }
     if(Mode==3)
    {
      
        ShortcutActionFW3();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }
 if(Mode==4)
    {
      
        ShortcutActionFW4();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }


  }

   if(NewValue<OldValue) //AntiClockwise turn
  {
    Serial.println("Backward");
    if(Mode==0)
    {
        ShortcutActionBW0();
        //if(NewValue<0){encoderValue=0;}
        AnimationStop(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    if(Mode==1)
    {
        ShortcutActionBW1();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    if(Mode==2)
    {
        ShortcutActionBW2();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    if(Mode==3)
    {
        ShortcutActionBW3();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

      if(Mode==4)
    {
        ShortcutActionBW4();
        AnimationContinuous(NewValue, Red[Mode], Green[Mode], Blue[Mode]);
    }

    
  }
  
  lastEncoded = encoded; //store this value for next time
  
}
