
#include <Servo.h>

Servo myservo;  // create servo object to control a servo

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(90);  
}

void loop() {
  myservo.write(70);   // scale it for use with the servo (value between 0 and 180)                // sets the servo position according to the scaled value
  delay(1000);
  myservo.write(110);   // scale it for use with the servo (value between 0 and 180)                // sets the servo position according to the scaled value
  delay(1000);                           // waits for the servo to get there
}
