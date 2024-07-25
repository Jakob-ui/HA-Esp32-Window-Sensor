#include <HA.h>

void onDisconnected();

#define BROKER_ADDR     IPAddress(192,168,68,104)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
unsigned long lastReadAt = millis();
bool lastInputState = false;

WiFiClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

int timeout;
int timeouttime;
bool wlanflag;

void mqttsetup(Secrets* config)
{

  WiFi.macAddress(mac);
  device.setUniqueId(mac, sizeof(mac));
  device.enableSharedAvailability();
  device.enableLastWill();

  Serial.begin(115200);
  WiFi.begin(config->ssid,config->pass);

  device.setName("Window_Sensor");
  device.setSoftwareVersion("1.0.1");
  mqtt.onDisconnected(onDisconnected);

  if(mqtt.begin(BROKER_ADDR, config->port, config->mqtt_user.c_str(), config->mqtt_password.c_str()) == false){
    Serial.println("Connection with MQTT Broker failed");
    MqttDisconnect("MQTT Failed");
  }
  
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

void sensorSetup(HABinarySensor sensor, bool state, const char* sensor_name, const char* sensor_device, const char* sensor_icon){
  sensor.setCurrentState(state);
  sensor.setName(sensor_name);
  sensor.setDeviceClass(sensor_device);
  sensor.setIcon(sensor_icon);
}

void sensorUpdate(HABinarySensor sensor, bool state){ 
  sensor.setState(state, true);
}


void mqttloop() {
   mqtt.loop();
}

void onDisconnected(){
  MqttDisconnect("Mqtt Disconnected");
}