#include <Wire.h>
#include <cvzone.h>

SerialData serialData(5, 1); //(numOfValsRec,digitsPerValRec)
// no of values recieved : 1:(0-1);2(1-99)
//here we take 2 as we have range between 1-5
int valsRec[5]; // array of int with size numOfValsRec 
int red = 2;
int yellow = 3;
int blue = 4;
int green = 5;
int white = 6;

void setup() {
  serialData.begin(9600);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(white, OUTPUT);
}

void loop(){
  int v;
  
  serialData.Get(valsRec);//get value from python code

  if(valsRec[0] == 1)
  {
    digitalWrite(red, HIGH);
  }
  else 
  {
    digitalWrite(red, LOW);
  }
  if(valsRec[1] == 1)
  {
    digitalWrite(yellow, HIGH);
  }
  else 
  {
      digitalWrite(yellow, LOW);
  }
  if(valsRec[2] == 1)
  {
    digitalWrite(blue, HIGH);
  }
  else 
  {
    digitalWrite(blue, LOW);
  }
  if(valsRec[3] == 1)
  {
    digitalWrite(green, HIGH);
  }
  else
  {
    digitalWrite(green, LOW);
  } 

  if(valsRec[4] == 1)
  {
    digitalWrite(white, HIGH);
  }
  else
  {
    digitalWrite(white, LOW);
  }
}