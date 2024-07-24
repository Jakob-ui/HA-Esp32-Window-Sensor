#include <HA.h>

void onDisconnected();

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

const char* ssid = "user";          
const char* pass = "password";

int timeout;
int timeouttime;
bool wlanflag;

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
  mqtt.onDisconnected(onDisconnected);

  mqtt.begin(BROKER_ADDR, port, mqtt_user.c_str(), mqtt_password.c_str());
  
  while (WiFi.status() != WL_CONNECTED) {
    if(timeout <= 4){
    timeout ++;
    timeouttime = 0;
    WlanConnecting("Wifi connect", 4000);
    delay(200);
    }else{
      WlanConnectionFailed();
      updateDisplay();
      Serial.println("Connection failed");
      delay(400);
      timeouttime++;
      if(timeouttime >= 5){
        timeout = 0;
      }
  }
}
}

void sensorSetup(HABinarySensor sensor, int pin, bool state, String sensor_name, String sensor_device, String sensor_icon){
  sensor.setCurrentState(state);
  sensor.setName("Button");
  sensor.setDeviceClass("door");
  sensor.setIcon("mdi:fire");
}

void sensorUpdate(HABinarySensor sensor, int pin){ 
  sensor.setState(digitalRead(pin), true);
}


void mqttloop() {
   mqtt.loop();
}

void onDisconnected(){
  MqttDisconnect("Mqtt Disconnected");
}