#include "Arduino.h"
#include "WiFiManager.h"
#include "esp_wifi.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

WiFiManager::WiFiManager(){
}

void WiFiManager::enableOTA(){
  ArduinoOTA.begin();
  ArduinoOTA.onStart([&]() {
      Serial.println("ArduinoOTA starting..");
  });

  ArduinoOTA.onProgress([&](unsigned int progress, unsigned int total) {
      Serial.println(progress);
  });

  ArduinoOTA.onError([&](ota_error_t err) {
      Serial.print("OTA error: ");
      Serial.println(err);
      ESP.restart();
  });

  ArduinoOTA.onEnd([&]() {
      Serial.println("OTA complete.");
  });

  isEnableOTA=true;

}

bool WiFiManager::init(){

  String msg = "WiFi Setup\nConnecting to ";
  Serial.print("WiFi Setup => connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin (ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED && reconnect<MAX_RECONNECT) { 
    Serial.print("Connection Failed! try to reconnect:");
    Serial.println(reconnect);
    reconnect++;
  }

  reconnect=0;

  if(WiFi.isConnected()){
    Serial.println("WiFi ready");
    WiFi.setHostname("esp32potp");
    Serial.println(WiFi.localIP().toString());

    if(!isEnableOTA)enableOTA();
    Serial.println("OTA ready");

    isWifiEnable=true;
  }
  else {
    Serial.println("WiFi setup failed!");
    isWifiEnable = false;
  }

  return isWifiEnable;

}

void WiFiManager::disableWifi(){
  if(isWifiEnable){ 
    Serial.println("Disabling WiFi..");
    esp_wifi_set_ps(WIFI_PS_MODEM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    isWifiEnable=false;
    ESP.restart();
  }
}
