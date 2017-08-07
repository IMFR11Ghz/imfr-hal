/*
// The ADC input range (or gain) can be changed via the following
// functions, but be careful never to exceed VDD +0.3V max, or to
// exceed the upper and lower limits if you adjust the input range!
// Setting these values incorrectly may destroy your ADC!
//                                                                ADS1015  ADS1115
//                                                                -------  -------
// ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
// ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
// ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
// ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
// ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
// ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  

 */


#include "A4988.h"
#include "DRV8825.h"
#include "DRV8834.h"
#include <Arduino.h>
#include <Adafruit_ADS1015.h>

#include "WiFiManager.h"
const int DIR = 7;
const int STEP = 6;

Adafruit_ADS1115 ads;
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
//
#define MODE0 11
#define MODE1 10
#define MODE2 9

DRV8825 stepper(MOTOR_STEPS, DIR, STEP, MODE0, MODE1, MODE2);
WiFiManager wifi;
int stop = 0;
int rotate = 360;
float voltage = 0.0;
int16_t adc0;

void setup() {
  /*
   * Set target motor RPM.
   */
  stepper.setRPM(100);
  stepper.setMicrostep(2); // micro step config

  Serial.begin(115200);
  Serial.println("ESP32 setup ready..");
  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
  //ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  //ads.begin();
  wifi.init();  
}

void ads_read(void){
  adc0 = ads.readADC_SingleEnded(0);
  voltage = (adc0 * 0.0078125)/1000;
  Serial.print("AIN0: "); Serial.println(voltage,9);
  Serial.println(" ");
}

void motor_move(void) {
  Serial.println("servo rotate  360..");
  stepper.rotate(rotate);
  if (stop++ == 10) {
    stop = 0;
    delay(2000);
    rotate = -rotate;
  }
  delay(2000);
  Serial.println("servo rotate -360..");
  stepper.rotate(-360);
  delay(2000);
}

void loop(void) {
  if(wifi.isWifiEnable)
      ArduinoOTA.handle();
  delay(10);
  //ads_read();
  motor_move();

}


