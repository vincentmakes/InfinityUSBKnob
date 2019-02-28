#include "HID-Project.h"
//#include "Keyboard.h" //already included in HID-Project. 
#include "Adafruit_NeoPixel.h"


#define PIN  10//to change to DI
#define NUMPIXELS      16 //
volatile byte mybrightness = 32;
 
Adafruit_NeoPixel pixels=Adafruit_NeoPixel(NUMPIXELS,PIN,NEO_GRB+NEO_KHZ800);




int encoderPinA = 2;
int encoderPinB = 3;
int buttonPin = 1;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;
long OldValue = 0;
long NewValue =0;

int lastMSB = 0;
int lastLSB = 0;

long readEncoderValue(void){
    return encoderValue/4;
}

boolean isButtonPushDown(void){
  if(!digitalRead(buttonPin)){
    delay(5);
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


  Consumer.begin();

  
  pixels.setBrightness(mybrightness);
  pixels.begin();
  pixels.show();
}

void loop(){ 
  //Do stuff here

  if(isButtonPushDown()){
    Serial.println("you push button down!!!");
Keyboard.begin();
Keyboard.press(150);
delay(100);
Keyboard.releaseAll();//release

//207->F1 (lum-)
//208->F2 (lum+)
//204-> 5 fingers keypad=fn+F11
//224 return
  }

 
  
  
  
 
  delay(50); //just here to slow down the output, and show it will work  even during a delay

}


void updateEncoder(){
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
  
  if(NewValue>OldValue)
  {
    Serial.println("Forward");

    byte numLed = (NewValue);
     Serial.println(numLed);
     
    for(int i=0;i<numLed;i++) { 
       pixels.setPixelColor(i, NewValue,114,255, mybrightness); 
    }
    for(int i=numLed;i<NUMPIXELS;i++) { 
       pixels.setPixelColor(i, 0,0,0, 0); 
    }
    pixels.show();
    Consumer.write(MEDIA_VOLUME_UP);
    //Keyboard.begin();
    //Keyboard.press(KEY_LEFT_GUI);//Redo
    //Keyboard.press(KEY_LEFT_SHIFT);//SHIFT+CMD+z
    //Keyboard.write('c');
    //Keyboard.press(NewValue);
    delay(100);
    Keyboard.releaseAll();//release
  }

   if(NewValue<OldValue)
  {
    Serial.println("Backward");

    Consumer.write(MEDIA_VOLUME_DOWN);
    byte numLed = (NewValue);
     Serial.println(numLed);
     
    for(int i=numLed;i<NUMPIXELS;i++) { 
       pixels.setPixelColor(i, 0,0,0,0); 
    }
    pixels.show();
    //Keyboard.begin();
    //Keyboard.press(KEY_LEFT_GUI);//Undo CMD+z
    //Keyboard.write('z');
    //delay(100);
    //Keyboard.releaseAll();//release
  }
  
  lastEncoded = encoded; //store this value for next time

  
}
