#include <TimeLib.h>
#include <ADC.h>
int value = 0;
int value2 = 0;
char c=0;
int timeStart =0;
int timeCur =0;
int nowDelta =0;

const int readPin = A9; // ADC0

//time_t lcd_time;
ADC *adc = new ADC(); // adc object

time_t getTeensy3Time(){
  return Teensy3Clock.get();
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
  return pctime;
}

void setupRTC(){
    setSyncProvider(getTeensy3Time);
    Serial.begin(115200);
    while (!Serial);  // Wait for Arduino Serial Monitor to open
    delay(100);
    if (timeStatus()!= timeSet) {
      Serial.println("Unable to sync with the RTC");
    } else {
      Serial.println("RTC has set the system time");
    }
    timeStart = now();
}

void setup() {
    // set the current time
    setupRTC();
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT);

    ///// ADC0 ////
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
    adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->setAveraging(16); // set number of averages
    adc->setResolution(16); // set bits of resolution

    // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

    adc->startContinuous(readPin, ADC_0);


    delay(500);
}

void loopGraph(){
// sample every second 
    if( now() - nowDelta >= 1 ){
        nowDelta = now();
      	timeCur = nowDelta - timeStart;
    	Serial.print(timeCur * 1.0 );
    	Serial.print(" ");
    	value = (uint16_t)adc->analogReadContinuous(ADC_0); 
    	Serial.println(value*3.3/adc->getMaxValue(ADC_0), DEC);
    }
}
void loop(){
    loopGraph();
}
