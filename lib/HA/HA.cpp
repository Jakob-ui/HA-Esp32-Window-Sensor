#include <ArduinoHA.h>
#include <WiFi.h>
#include <SPI.h>
#include <Config.h>


char ssid[] = "Wlan-Magenta_G";          
char pass[] = "Adbe1788###";

int status = WL_IDLE_STATUS;
IPAddress server(80,109,160,170);

WiFiClient client;

void setup()
{
    Serial.begin(115200);
    Serial.println("Attempting to connect to WPA network...");
    Serial.print("SSID: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);
    if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a wifi connection");
    while(true);
    }
    else {
        Serial.println("Connected to wifi");
        Serial.println("\nStarting connection...");
        // if you get a connection, report back via serial:
        if (client.connect(server, 1882)) {
        Serial.println("connected");
        // Make a HTTP request:
        client.println("GET /search?q=arduino HTTP/1.0");
        client.println();
    }
  }
}


void loop() {
   if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
}





/* 
#include <ArduinoHA.h>
#include <WiFi.h>
#include <SPI.h>
#include <Config.h>

#define INPUT_PIN       9
#define BROKER_ADDR     IPAddress(192,168,68,104)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
unsigned long lastReadAt = millis();
bool lastInputState = false;
String ssid = "Wlan-Magenta_G";
String pass = "Adbe1788###";

WiFiClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

int port = 1883;
String mqtt_user = "admin";
String mqtt_password = "Schnecke123";

// "myInput" is unique ID of the sensor. You should define you own ID.
HABinarySensor sensor("Window Sensor");

void setup() {
    pinMode(INPUT_PIN, INPUT_PULLUP);
    lastInputState = digitalRead(INPUT_PIN);

    WiFi.macAddress(mac);
    device.setUniqueId(mac, sizeof(mac));
    device.setName("Window Sensor");
    device.setSoftwareVersion("1.0.0");
    device.enableSharedAvailability();
    device.enableLastWill();

    // optional device's details
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // optional properties
    sensor.setCurrentState(lastInputState);
    sensor.setName("Door sensor");
    sensor.setDeviceClass("door");
    sensor.setIcon("mdi:fire");

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    WiFi.begin(ssid, pass);
    mqtt.loop();

    // reading the digital input of the Arduino device
    if ((millis() - lastReadAt) > 30) { // read in 30ms interval
        // library produces MQTT message if a new state is different than the previous one
        sensor.setState(digitalRead(INPUT_PIN));
        lastInputState = sensor.getCurrentState();
        lastReadAt = millis();
    }
}
*/