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
#include <NtpClientLib.h>
#include <Adafruit_ADS1015.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <time.h>
#include <ctime>
#include <PubSubClient.h>
#include <ArduinoJson.h>

//Everything motor
//
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


long lastMsg = 0;
char msg[75];
int value = 0;

const char * host = "192.168.0.3"; // ip or dns
DynamicJsonBuffer jbuf;
char server_mqtt[] = "sensors.trace.global";

WiFiMulti Wifi;
WiFiClient esp;
PubSubClient mqtt(esp);
boolean syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

int8_t timeZone = -5;
int8_t minutesTimeZone = 0;
bool wifiFirstConnected = false;
time_t now_time;
long now_millis;
long now_millis_send;
long now_s;
float voltage = 0.0;
int16_t adc0;

const double accuG23= 0.1875; //G2/3
const double accuG1= 0.125; //G1
const double accuG2= 0.0625; //G2
const double accuG4= 0.03125; //G4
const double accuG8= 0.015625; //G8
const double accuG16= 0.0078125;//G16

const double sampling_rate = 5.0;

//CONFIG gain, accuracy and sazmpling delta
int ns=0;
bool wifi=false;

const int DIR = 18;
const int STEP = 19;
DRV8825 stepper(MOTOR_STEPS, DIR, STEP, MODE0, MODE1, MODE2);

int stop = 0;
int rotate = 360;
void setup_motor(void){
  stepper.setRPM(1);
  stepper.setMicrostep(2); // micro step config
}

void onEvent (system_event_id_t event, system_event_info_t info) {
    Serial.printf ("[WiFi-event] event: %d\n", event);

    switch (event) {
    case SYSTEM_EVENT_STA_CONNECTED:
        Serial.printf ("Connected to %s\r\n", info.connected.ssid);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.printf ("Got IP: %s\r\n", IPAddress (info.got_ip.ip_info.ip.addr).toString ().c_str ());
        Serial.printf ("Connected: %s\r\n", WiFi.status () == WL_CONNECTED ? "yes" : "no");
        wifiFirstConnected = true;
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.printf ("Disconnected from SSID: %s\n", info.disconnected.ssid);
        Serial.printf ("Reason: %d\n", info.disconnected.reason);
        //NTP.stop(); // NTP sync can be disabled to avoid sync errors
        break;
     default:
        break;
    }
    Serial.println(); 
}

void processSyncEvent (NTPSyncEvent_t ntpEvent) {
    if (ntpEvent) {
        Serial.print ("Time Sync error: ");
        if (ntpEvent == noResponse)
            Serial.println ("NTP server not reachable");
        else if (ntpEvent == invalidAddress)
            Serial.println ("Invalid NTP server address");
    } else {
        Serial.print ("Got NTP time: ");
        Serial.println (NTP.getTimeDateString (NTP.getLastNTPSync ()));
    }
}

void parseJsonString(const char * payload_js) {
   JsonObject& root = jbuf.parseObject(payload_js);
   String data = "";
   root.printTo(data);
   Serial.println(data);
}

void callback_mqtt(char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);
  const char* payload_js=(char *)payload;
  parseJsonString(payload_js);
}

  
void setup_wifi(void) {
    delay(10);
    Wifi.addAP("Home47", "#1018405230#");
    Serial.println();
    Serial.print("Wait for WiFi... ");
    Serial.println();
    WiFi.onEvent (onEvent);
    while(Wifi.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    /*
    NTP.onNTPSyncEvent ([](NTPSyncEvent_t event) {
        ntpEvent = event;
        syncEventTriggered = true;
    });
    NTP.begin ("pool.ntp.org", timeZone, true, minutesTimeZone);
	    NTP.setInterval (63);
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    delay(500);
    */
}


void reconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    bool connection = mqtt.connect("temp","acurite","tr4c32018");
    //bool connection = mqtt.connect("temp");
    if (connection) {
      Serial.println("connected");
      //mqtt.subscribe("sun");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup_socket_node(){
     setup_wifi();
     mqtt.setServer(server_mqtt, 1883);
     mqtt.setCallback(callback_mqtt);
}

const long DELTA_500 = 4;
long delta=DELTA_500;
const int gain=0;
double accuracy;
Adafruit_ADS1115 ads;
String pkg="";

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
  //setup_socket_node();
  setup_ads(); 
  //setup_motor();
  now_millis = millis();
  now_millis_send = millis();
}

String adc_packet_data(double t, float v){
   String input = "[" + String(t) + "," + String(v,7) + "]";
   return input;

}


void send_data(String buffer){
	const char *payload = buffer.c_str();
	Serial.println(payload);
	mqtt.publish("telescope",payload);
}

void prepare_data(void){
	int len = pkg.length();
	pkg.remove(len-1);
	pkg = "{" + pkg + "}";
	String complete_pkg = pkg;
	send_data(complete_pkg);
	pkg = "";
}

void loopGraphAcu(void){
    long cur_millis = millis();
    if(now_millis + delta <=  cur_millis ){
	float v_i= ads_read();
	double t_i = (now_millis / 1000.0);
  	//Serial.print(t_i); Serial.print(" ");  Serial.print(" ");  Serial.println(v_i,9);
	if(ns >= 5){
	    ns=0;
	    prepare_data();
	}
	else{
	    ns+=1;
	    pkg = pkg + adc_packet_data(t_i,v_i ) + ",";
      	    now_millis = millis();
	}

   }
}

void loop_motor(){
  Serial.println("servo rotate  360..");
  stepper.rotate(-360);
  delay(2000);
}

void loop_connection(){
  if (!mqtt.connected()) { reconnect();}
  mqtt.loop();
  //Serial.println(now());
  //Serial.println (NTP.getTimeDateString (NTP.getLastNTPSync ()));
}

void loop(void) {
  //loop_connection();
  loopGraphAcu();
  //samples_per_second();
  //loop_motor();
}
