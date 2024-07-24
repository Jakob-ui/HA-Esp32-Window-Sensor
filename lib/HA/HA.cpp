#include <HA.h>

#define BROKER_ADDR     IPAddress(192,168,68,104)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
unsigned long lastReadAt = millis();
bool lastInputState = false;

WiFiClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

String mqtt_user = "user";
String mqtt_password= "password";
int port = 1883;

const char* ssid = "Wlan-SSID";          
const char* pass = "Wlan-Password";

void mqttsetup()
{
  WiFi.macAddress(mac);
  device.setUniqueId(mac, sizeof(mac));
  device.enableSharedAvailability();
  device.enableLastWill();

  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  device.setName("Window_Sensor");
  device.setSoftwareVersion("1.0.1");

  mqtt.begin(BROKER_ADDR, port, mqtt_user.c_str(), mqtt_password.c_str());
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("Connected!");
}

void sensorSetup(HABinarySensor sensor, int pin, bool state, String sensor_name, String sensor_device, String sensor_icon){
  sensor.setCurrentState(state);
  sensor.setName("Button");
  sensor.setDeviceClass("door");
  sensor.setIcon("mdi:fire");
}

void sensorUpdate(HABinarySensor sensor, int pin){ 
  if(digitalRead(pin) == HIGH) 
  sensor.setState(true);
  else 
  sensor.setState(false);
   
}


void mqttloop() {
   mqtt.loop();
}