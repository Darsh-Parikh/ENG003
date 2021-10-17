#include<Servo.h>

const char night = 'n';
const char day = 'd';

const int servoPin = 2; // the Servo motor pin
Servo timeTeller; // initializes the motor
const int led1Pin = 3;   // an LED pin
const int led2Pin = 4;  // another LED pin
const int ldrPin = A0;  // the LDR pin
const int ldrCutoff = 300; // the Cutoff Point for the photoresistor. Indicates if it is Night or Day
bool isOn = false; // A way to know if the LEDs are already on or not.

void setup() {
  Serial.begin(9600); //Set initial value for photoresistor
  timeTeller.attach(servoPin, 500, 2500); // binds the servo to its digital pin, 
  pinMode(ldrPin, INPUT); 
  pinMode(servoPin, OUTPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
}

void moveMotor(char time) {
  // In the physical display, Night (‘n’) is on the left side, Day (‘d’) is on the right
  if (time == day) {
    for (int pos = 0; pos < 180; ++pos) {
      timeTeller.write(pos);
      delay(15); // wait 15 ms for servo to reach the position
    }
  } else if (time == night) {
    for (int pos = 180; pos > 0; --pos) {
      timeTeller.write(pos);
      delay(15); // wait 15 ms for servo to reach the position
    }
  }
}

void changeLEDs(bool state) {
  digitalWrite(led1Pin, state);
  digitalWrite(led2Pin, state);
}

void loop() {
  int sensorValue = analogRead(ldrPin); //Reads sensor value
  Serial.println(sensorValue); //Prints photoresistor value
  
  if ( !isOn &&  sensorValue < ldrCutoff) {
    changeLEDs(HIGH); // Turns on all the LEDs
    moveMotor(night); // Moves the Arrow hand to ‘Night’
    isOn = true;
  } else if ( isOn && sensorValue > ldrCutoff) {
    changeLEDs(LOW); // Turns off all the LEDs
    moveMotor(day); // Moves the Arrow hand to ‘Day’
    isOn = false;
  }
}
