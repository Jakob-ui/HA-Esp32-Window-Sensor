#ifndef oled_Display_H
#define oled_Display_H
#include <Arduino.h>
#include <spi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "globals.h"

void oledSetup();
void updateDisplay();
void displayCenteredText(const char *text, uint8_t textSize, bool alarm);
void turnOffDisplay();
void turnONDisplay();
void WlanConnecting(const char *text, int timeout);
void MqttDisconnect(const char *text);
void WlanConnectionFailed();
void drawLinesAndIcon(bool alarm);
#endif