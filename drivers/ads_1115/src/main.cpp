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

#include <WiFi.h>
#include <WiFiMulti.h>

#include <time.h>
#include <ctime>
#include <PubSubClient.h>
#include <ArduinoJson.h>



// wifi related stuff 
WiFiMulti WiFiMulti;
WiFiClient espClient;
WiFiServer server(80);
PubSubClient client(espClient);
DynamicJsonBuffer jbuf;

long lastMsg = 0;
char msg[75];
int value = 0;
const uint16_t port = 6666;
const char * host = "192.168.0.3"; // ip or dns
const char* mqtt_server2 = "192.168.0.6";
const char* mqtt_server = "192.168.43.186";
// Use WiFiClient class to create TCP connections


Adafruit_ADS1115 ads;

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
const long DELTA50 = 20;
const long DELTA10 = 100;
const long DELTA1 = 1000;
const long DELTA2 = 2000;
const long DELTA_500 = 2;
const long DELTA20 = 50;
const long DELTA_1000 = 0.5;
//CONFIG gain, accuracy and sazmpling delta

long delta=DELTA_500;
const int gain=1;
double accuracy= accuG1;
double average_voltage =0;
int nsamples =0;
int ns =0;
double cur_v =0.0;
bool wifi=false;

void callback_mqtt(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  const char*jb=(char *)payload;
  JsonObject& root = jbuf.parseObject(jb);
}


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
/*
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}
*/
void setup_wifi(void) {
    delay(10);
    // We start by connecting to a WiFi network
    WiFiMulti.addAP("radio", "12345678");
    Serial.println();
    Serial.print("Wait for WiFi... ");
    Serial.println();

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    delay(500);
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("sun", "heartbeat");
      client.subscribe("sun");

      // ... and resubscribe
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup_socket_node(){
     setup_wifi();
     client.setServer(mqtt_server, 1883);
     client.setCallback(callback_mqtt);
}



void setup() {

  Serial.begin(115200);
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  //setup_wifi();

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

void samples_per_second(){
    long cur_millis = millis();
    if(now_s + 1000 <=  cur_millis ){
	now_s = millis();
  	//Serial.print("samples per second "); Serial.println(ns);
	ns = 0;
	
   }
}
void loopGraphAcu(void){
    long cur_millis = millis();
    if(now_millis + delta <=  cur_millis ){
        cur_v= ads_read();
	double time_secs = (now_millis / 1000.0);
	double v = 0.0;

        if( average_voltage > 0 && nsamples > 0 )
	     v = average_voltage * 1.0 / nsamples;
	else
	     v = cur_v;


  	Serial.print(time_secs); Serial.print(" ");  Serial.print(" ");  Serial.println(v,9);
	average_voltage = v;

	//snprintf (msg, 75, ".2%f .2%f", time_secs,v);
  	//client.publish("sun", msg);

	now_millis = millis();
	ns+=1;
   }
   else{
        average_voltage += cur_v;
	nsamples += 1;
   }
//client.stop();
}


void loop(void) {
  //if (!client.connected()) {
  //  reconnect();
  //}
  //client.loop();
  samples_per_second();
  loopGraphAcu();
}
