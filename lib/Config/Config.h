#pragma once
#include <ArduinoJson.h>
#include <IPAddress.h>
#include <stdint.h>

struct Secrets
{
    String ssid;
    String pass;
    String mqtt_user;
    String mqtt_password;
    int port;
};

bool deserializeJsondoc(Secrets* secrets);