#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
/*
class Secrets {
public:
  int port;
  String mqtt_user;
  String mqtt_password;
  String ssid;
  String pass;

  Secrets() : port(0), mqtt_user(""), mqtt_password(""), ssid(""), pass("") {}

  void deserializeJsondoc() {
    if (!SPIFFS.begin(true)) {
      Serial.println("An error has occurred while mounting SPIFFS");
      return;
    }
    
    File file = SPIFFS.open("/Secrets.json", "r");
    if (!file || file.isDirectory()) {
      Serial.println("Failed to open file for reading");
      return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, file);
    
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    this->mqtt_user = doc["MQtt"][0]["user"].as<String>();
    this->mqtt_password = doc["MQtt"][0]["password"].as<String>(); 

    JsonObject wlan = doc["Wlan"][0];
    this->port = wlan["port"].as<int>();
    this->ssid = wlan["ssid"].as<String>();
    this->pass = wlan["pass"].as<String>();

    file.close();
  }
};
*/