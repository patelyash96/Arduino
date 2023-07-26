#include <PCA9685.h>
#include <Wire.h>
#include <cvzone.h>

PCA9685 driver;
PCA9685_ServoEval pwmServo(80, 500); // (-deg, +deg) Max and Min degree

SerialData serialData(5, 1); //(numOfValsRec,digitsPerValRec) 5 data 0/1 value

int valsRec[5]; // array of int with size numOfValsRec $000000 [0,0,0,0,0,blank]

void setup() {
  
  Wire.begin(); // Wire must be started first for SDA SCL
  Wire.setClock(400000);// Supported baud rates are 100kHz,400kHz, and 1000kHz
  driver.resetDevices();// Software resets all PCA9685 devices on Wire line
  driver.init(); // Address pins A5-A0 set to B000000 init(B000000)
  driver.setPWMFrequency(60); // Set frequency to 50Hz
  
  //Set initial positon of hand
  driver.setChannelPWM(0, pwmServo.pwmForAngle(0));
  driver.setChannelPWM(1, pwmServo.pwmForAngle(0));
  driver.setChannelPWM(2, pwmServo.pwmForAngle(0));
  driver.setChannelPWM(4, pwmServo.pwmForAngle(50));
  driver.setChannelPWM(5, pwmServo.pwmForAngle(70));
  
  serialData.begin(9600);// receiving data
  delay(1000);
}

void loop(){
  int v;
  
  serialData.Get(valsRec);//get value from python code

  if(valsRec[0] == 0) // Thumb close
  {
    driver.setChannelPWM(0, pwmServo.pwmForAngle(90));
    delay(10);
  }
  else 
  {
   driver.setChannelPWM(0, pwmServo.pwmForAngle(0));
   delay(10);
  }
  if(valsRec[1] == 0) // Index Finger close
  {
    driver.setChannelPWM(1, pwmServo.pwmForAngle(70));
    delay(10);
  }
  else 
  {
   driver.setChannelPWM(1, pwmServo.pwmForAngle(0));
   delay(10);
  }
  if(valsRec[2] == 0) // Middle Finger close
  {
    driver.setChannelPWM(2, pwmServo.pwmForAngle(60));
    delay(10);
  }
  else 
  {
   driver.setChannelPWM(2, pwmServo.pwmForAngle(0));
   delay(10);
  }
  if(valsRec[3] == 0) // Ring Finger close
  {
    driver.setChannelPWM(4, pwmServo.pwmForAngle(0));
    delay(10);
  }
  else 
  {
   driver.setChannelPWM(4, pwmServo.pwmForAngle(50));
   delay(10);
  }
  if(valsRec[4] == 0) // Pinky/Little Finger close
  {
    driver.setChannelPWM(5, pwmServo.pwmForAngle(0));
    delay(10);
  }
  else 
  {
   driver.setChannelPWM(5, pwmServo.pwmForAngle(70));
   delay(10);
  }
}