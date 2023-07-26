#include <Wire.h>
#include "PCA9685.h"

PCA9685 driver;

// PCA9685 outputs = 12-bit = 4096 steps
// 2.5% of 20ms = 0.5ms ; 12.5% of 20ms = 2.5ms
// 2.5% of 4096 = 102 steps; 12.5% of 4096 = 512 steps

PCA9685_ServoEval pwmServo(100, 500); // (-deg, +deg)

void setup() {

  Wire.begin(); // Wire must be started first
  Wire.setClock(400000);// Supported baud rates are 100kHz,400kHz, and 1000kHz
  driver.resetDevices();// Software resets all PCA9685 devices on Wire line
  driver.init(); // Address pins A5-A0 set to B000000 init(B000000)
  driver.setPWMFrequency(60); // Set frequency to 50Hz
  driver.setChannelPWM(0, pwmServo.pwmForAngle(0));
  driver.setChannelPWM(1, pwmServo.pwmForAngle(0));
  driver.setChannelPWM(2, pwmServo.pwmForAngle(0));
  driver.setChannelPWM(4, pwmServo.pwmForAngle(50));
  driver.setChannelPWM(5, pwmServo.pwmForAngle(70));
  delay(1000);
}

void loop() {
 driver.setChannelPWM(0, pwmServo.pwmForAngle(0));
 delay(10);
 driver.setChannelPWM(1, pwmServo.pwmForAngle(0));
 delay(10);
 driver.setChannelPWM(2, pwmServo.pwmForAngle(0));
 delay(10);
 driver.setChannelPWM(4, pwmServo.pwmForAngle(50));
 delay(10);
 driver.setChannelPWM(5, pwmServo.pwmForAngle(70));
 delay(1000);
 driver.setChannelPWM(0, pwmServo.pwmForAngle(70));
 delay(10);
 driver.setChannelPWM(1, pwmServo.pwmForAngle(70));
 delay(10);
 driver.setChannelPWM(2, pwmServo.pwmForAngle(70));
 delay(10);
 driver.setChannelPWM(4, pwmServo.pwmForAngle(0));
 delay(10);
 driver.setChannelPWM(5, pwmServo.pwmForAngle(0));
 delay(1000);
}