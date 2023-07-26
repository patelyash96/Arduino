#include <Wire.h>
#include <cvzone.h>

SerialData serialData(1, 1); //(numOfValsRec,digitsPerValRec)
int valsRec[1]; // array of int with size numOfValsRec 

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  serialData.begin();
}

void loop() {

  serialData.Get(valsRec);
  digitalWrite(LED_BUILTIN, valsRec[0]);
  delay(10);
}
