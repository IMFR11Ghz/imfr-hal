/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include "A4988.h"
#include "DRV8825.h"
#include "DRV8834.h"
#include <Arduino.h>

const int DIR = 14;
const int STEP = 12;

#define MOTOR_STEPS 200

// microstep control for DRV8834
//#define M0 2
//#define M1 3
// DRV8834 stepper(MOTOR_STEPS, DIR, STEP, M0, M1);

// microstep control for A4988
// #define MS1 10
// #define MS2 11
// #define MS3 12
// A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3);

// microstep control for DRV8825
// same pinout as A4988, different pin names, supports 32 microsteps
#define MODE0 2
#define MODE1 3
#define MODE2 13
DRV8825 stepper(MOTOR_STEPS, DIR, STEP, MODE0, MODE1, MODE2);

int stop = 0;
int rotate = 360;

void setup() {
  /*
   * Set target motor RPM.
   */
  stepper.setRPM(100);
  stepper.setMicrostep(2); // micro step config

  Serial.begin(115200);
  Serial.println("ESP32 setup ready..");
}

void loop() {
  /*
   * The easy way is just tell the motor to rotate 360 degrees at 1rpm
   */
  Serial.println("servo rotate  360..");
  stepper.rotate(rotate);
  if (stop++ == 10) {
    stop = 0;
    delay(2000);
    rotate = -rotate;
  }

  /*
  delay(2000);
  Serial.println("servo rotate -360..");
  stepper.rotate(-360);
  delay(2000);
  */
}

