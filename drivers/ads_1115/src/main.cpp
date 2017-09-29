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

#include <time.h>
#include <ctime>

Adafruit_ADS1115 ads;
#define MOTOR_STEPS 200

time_t now_time;
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
//
#define MODE0 27
#define MODE1 26
#define MODE2 25
const int DIR = 32;
const int STEP = 33;


int stop = 0;
int rotate = 360;
float voltage = 0.0;
int16_t adc0;

 // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  //
  //

//double accuracy= 0.1875; //G2/3
const double accuracy= 0.125; //G2/3
//const double accuracy= 0.0625; //G1
//const double accuracy= 0.03125; //G2
//const double accuracy= 0.015625; //G4
//const double accuracy= 0.0078125;//G16
const double sampling_rate = 5.0;

double ads_read(void){
  int adc0 = ads.readADC_SingleEnded(0);
  double voltage = (adc0 * accuracy )/1000.0;
  return voltage;
}

time_t getTime(void){
   time_t cur_time;
   time(&cur_time);
   return cur_time;
}
// idea 1, multiply time lets say: get time increments in 1, so if is multiply per 10 that will sample faster 
void setup() {

  Serial.begin(115200);
  Serial.println("ESP32 setup ready..");
  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
  //ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  //ads.setGain(GAIN_FOUR);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  ads.setGain(GAIN_ONE);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  ads.begin();
  now_time = getTime();
}

void loopGraph(void){
    if(now_time + 1 <= ( getTime() * sampling_rate )){
	now_time =getTime();
  	double v= ads_read();
	// radio csv format
  	//Serial.print(now_time); Serial.print(","); Serial.println(v,9);
	// gnu_plot format
  	Serial.print(now_time *1.0 / sampling_rate); Serial.print(" "); Serial.println(v,9);

   }
}


void loop(void) {
  loopGraph();
}



