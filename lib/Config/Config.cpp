#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "Config.h"

  bool deserializeJsondoc(Secrets* secrets) {
    if (!SPIFFS.begin(true)) {
      Serial.println("An error has occurred while mounting SPIFFS");
      return false;
    }
    
    File file = SPIFFS.open("/Secrets.json", "r");
    if (!file || file.isDirectory()) {
      Serial.println("Failed to open file for reading");
      return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return false;
    }

    secrets->mqtt_user = doc["MQtt"]["user"].as<String>();
    secrets->mqtt_password = doc["MQtt"]["password"].as<String>(); 

    JsonObject wlan = doc["Wlan"];
    secrets->port = wlan["port"].as<int>();
    secrets->ssid = wlan["ssid"].as<String>();
    secrets->pass = wlan["pass"].as<String>();

    file.close();
    return true;
  }

