#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Config.h>
#include <ArduinoHA.h>
#include <Config.h>
#include <oled_Display.h>

void mqttsetup();
void mqttloop();
void sensorSetup(HABinarySensor sensor, int pin, bool state, String sensor_name, String sensor_device, String sensor_icon);
void sensorUpdate(HABinarySensor sensor, int pin);

/*
class HA
{
  public:
    HA(Secrets &config);

    private:
    
    Secrets &config;
};
*/