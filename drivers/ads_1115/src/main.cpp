
#include <Arduino.h>
#include <Adafruit_ADS1015.h>
#include <time.h>
#include <ctime>

long lastMsg = 0;
char msg[75];
int value = 0;
time_t now_time;
long now_millis;
long now_s;
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
double average_voltage =0;

int nsamples =0;
int ns =0;
float cur_v =0.0;

const long DELTA50 = 20;
const long DELTA10 = 100;
const long DELTA1 = 1000;
const long DELTA2 = 1000;
const long DELTA_3 = 333;
const long DELTA_500 = 2;
const long DELTA20 = 50;
const long DELTA_1000 = 0.5;

Adafruit_ADS1115 ads;

long delta=DELTA_500;
const int gain=0;
double accuracy;


float ads_read(void){
	const float v = ads.readADC_SingleEnded_V(0) ;
	return v;
}

void setup_ads(){

    ads.begin();
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

    ads.setSPS(ADS1115_DR_860SPS);          // 128SPS (default)
    ads.waitForConversion(); 
}

void setup() {

  Serial.begin(115200);
  setup_ads(); 
  now_millis = millis();
}


void samples_per_second(){
    long cur_millis = millis();
    if(now_s + 1000 <=  cur_millis ){
	now_s = millis();
  	Serial.print("samples per second "); Serial.println(ns);
	ns = 0;
	
   }
}

void loopGraph(void){
    long cur_millis = millis();
    if(now_millis + delta <=  cur_millis ){
        cur_v= ads_read();
  	Serial.println(cur_v);
	now_millis = millis();
   }
}

void loopGraphAcu(void){
    long cur_millis = millis();
    if(now_millis + delta <=  cur_millis ){
        cur_v= ads_read();
	double time_secs = (now_millis / 1000.0);
  	Serial.print(time_secs); Serial.print(" ");  Serial.print(" ");  Serial.println(cur_v,9);

	now_millis = millis();
	ns+=1;
   }
}


void loop(void) {
  //samples_per_second();
  loopGraphAcu();
}
