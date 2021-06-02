#include <Servo.h>
#include "SR04.h"
#define TRIG_PIN 12
#define ECHO_PIN 11


const int Servo_pin = 9;
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

const int MAX_Y = 1024;
const int MAX_DEGREE = 180;

Servo myservo;
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long distance;

int degree = 90;
bool inc = true;


void setup()
{
  Serial.begin(9600);
  myservo.attach(Servo_pin);
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);

  myservo.write(degree);
}

void loop()
{
//  float transition = float(analogRead(Y_pin)) / MAX_Y ;
//  int theta = (transition * MAX_DEGREE) ;
  
  myservo.write(degree);
  delay(10);
  distance=sr04.Distance();
  delay(10);
  Serial.print(degree);
  Serial.print("|");
  Serial.println(distance);

  if (inc)
    degree ++;
  else
    degree --;

  if (degree == 180)
    inc = false;
  if (degree == 0)
    inc = true;
}
