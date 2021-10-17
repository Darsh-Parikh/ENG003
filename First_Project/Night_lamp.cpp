#include<Servo.h>

const int servoPin = 2;    // the Servo motor pin
Servo timeTeller;          // initializes the motor
const int led1Pin = 3;     // an LED pin
const int led2Pin = 4;     // another LED pin
const int ldrPin = A0;     // the photoresistor
const int ldrCutoff = 300; // the Cutoff Point for the photoresistor. Indicates if it's Night or Day
bool isOn = false;         // A way to know if the LEDs are already on or not.

void setup() {
  Serial.begin(9600);                     //Set initial value for photoresistor
  timeTeller.attach(servoPin, 500, 2500); // binds the servo to its digital pin, 
  pinMode(ldrPin, INPUT);
  pinMode(servoPin, OUTPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
}

void moveMotor(char time) {
  // In the display, Night ('n') is on the right side, Day ('d') is on the left
  if (time == 'd') {
    for (int pos = 0; pos < 180; ++pos) {
      timeTeller.write(pos); // Moves the servo motor left by 1 degree
      delay(15);             // wait 15 ms for servo to reach the position
    }
  } else if (time == 'n') {
    for (int pos = 180; pos > 0; --pos) {
      timeTeller.write(pos); // moves the servo motor right by 1 degree
      delay(15);             // wait 15 ms for servo to reach the position
    }
  }
}

void changeLEDs(bool state) {
  digitalWrite(led1Pin, state);
  digitalWrite(led2Pin, state);
}

void loop() {
  int sensorValue = analogRead(ldrPin); //Reads sensor value
  Serial.println(sensorValue);          //Prints photoresistor value
  
  if ( !isOn &&  sensorValue < ldrCutoff) {
    changeLEDs(HIGH); // Turns on all the LEDs
    moveMotor('n');   // Moves the Arrow hand to 'Night'
    isOn = true;      // Notes that the lamp is now on
  } else if ( isOn && sensorValue > ldrCutoff) {
    changeLEDs(LOW);  // Turns off all the LEDs
    moveMotor('d');   // Moves the Arrow hand to 'Day'
    isOn = false;     // Notes that the lamp is now off
  }
}
