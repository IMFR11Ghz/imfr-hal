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

time_t now_time;
long now_millis;
const int DIR = 32;
const int STEP = 33;


int stop = 0;
int rotate = 360;
float voltage = 0.0;
int16_t adc0;

const double accuG23= 0.1875; //G2/3
const double accuG1= 0.125; //G1
const double accuG2= 0.0625; //G2
const double accuG4= 0.03125; //G4
const double accuG8= 0.015625; //G8
const double accuG16= 0.0078125;//G16

const double sampling_rate = 5.0;
const long DELTA50 = 20;
const long DELTA10 = 100;
const long DELTA1 = 1000;
const long DELTA2 = 2000;
const long DELTA500 = 2;
const long DELTA20 = 50;

//CONFIG gain, accuracy and sampling delta

long delta=DELTA20;
const int gain=1;
double accuracy= accuG1;

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

void setup() {

  Serial.begin(115200);
  Serial.println("ESP32 setup ready..");
  switch (gain){
	case 0:
             accuracy=accuG23;
             ads.setGain(GAIN_TWOTHIRDS);
	break;
	case 1:
             accuracy=accuG1;
             ads.setGain(GAIN_ONE);
	break;
	case 2:
             accuracy=accuG2;
             ads.setGain(GAIN_TWO);
	break;
	case 3:
             accuracy=accuG4;
             ads.setGain(GAIN_FOUR);
	break;
	case 4:
             accuracy=accuG8;
             ads.setGain(GAIN_EIGHT);
	break;
	case 5:
             accuracy=accuG16;
             ads.setGain(GAIN_SIXTEEN);
	break;
  }

  ads.begin();
  now_time = getTime();
  now_millis = millis();
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

void loopGraphAcu(void){

    if(now_millis + delta <=  millis() ){
  	double v= ads_read();
	double time_secs = getTime() + (now_millis / 1000.0);
  	Serial.print(time_secs); Serial.print(" "); Serial.println(v,9);
	now_millis = millis();
   }
}


void loop(void) {
  loopGraphAcu();
}



