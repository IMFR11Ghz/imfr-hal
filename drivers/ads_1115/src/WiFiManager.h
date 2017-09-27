/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef WifiManager_h
#define WifiManager_h

#include "Arduino.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define MAX_RECONNECT 1
const char* WIFI_SSID= "";
const char* WIFI_PASS= "";

class WiFiManager
{

  public:

   WiFiManager();
    
    bool init();

    void disableWifi();

    void enableOTA();

    bool isWifiEnable = false;

  private:

    // WiFi setup, define secrets on global enveiroment
    const char *ssid         = WIFI_SSID;
    
    const char *password     = WIFI_PASS;

    bool isEnableOTA = false;

    int reconnect = 0;

};

#endif
