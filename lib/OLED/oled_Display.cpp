#include <oled_Display.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned char fullBatteryIcon [] PROGMEM = {
	0x7f, 0xff, 0x40, 0x01, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0x40, 0x01, 0x7f, 0xff
};

const unsigned char middleBatteryIcon [] PROGMEM = {
	0x7f, 0xff, 0x40, 0x01, 0xc1, 0xdd, 0xc1, 0xdd, 0xc1, 0xdd, 0xc1, 0xdd, 0x40, 0x01, 0x7f, 0xff
};

const unsigned char lowBatteryIcon [] PROGMEM = {
	0x7f, 0xff, 0x40, 0x01, 0xc0, 0x1d, 0xc0, 0x1d, 0xc0, 0x1d, 0xc0, 0x1d, 0x40, 0x01, 0x7f, 0xff
};

const unsigned char soundAn [] PROGMEM = {
	0x00, 0x00, 0x01, 0x98, 0x03, 0x9c, 0x0f, 0x8c, 0x1f, 0x86, 0x7f, 0xb6, 0xff, 0xb7, 0xff, 0xb3, 
	0xff, 0xb7, 0xff, 0xb6, 0x7f, 0xb6, 0x1f, 0x8e, 0x0f, 0x8c, 0x03, 0x9c, 0x01, 0x98, 0x00, 0x10
};

const unsigned char soundAus [] PROGMEM = {
	0x00, 0x00, 0x00, 0x9f, 0x01, 0x9e, 0x07, 0x9c, 0x0f, 0xbc, 0x3f, 0xf6, 0x7f, 0xf6, 0x7f, 0xf2, 
	0x7f, 0xb6, 0x7f, 0xb6, 0x3f, 0xb6, 0x1f, 0x84, 0x3f, 0x8c, 0x73, 0x98, 0x60, 0x98, 0x40, 0x00
};

void drawLinesAndIcon();
void updateDisplay();


void oledSetup(){
  Serial.begin(9600);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  
  display.setTextSize(4);    
  display.setTextColor(SSD1306_WHITE); 
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds("J", 0, 0, &x1, &y1, &w, &h); 
  int16_t x = (SCREEN_WIDTH - w) / 2; 
  int16_t y = (SCREEN_HEIGHT - h) / 2;
  display.setCursor(x, y);    
  display.write('J');
  display.display(); 
  delay(2000);
  
  display.clearDisplay();
  display.display();
}


void displayCenteredText(const char* text, uint8_t textSize, bool alarm) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(WHITE);

  drawLinesAndIcon(alarm);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int16_t x = (SCREEN_WIDTH - w) / 2;
  int16_t y = (SCREEN_HEIGHT - h) / 2;

  display.setCursor(x, y);
  display.println(text);
  updateDisplay();
}

void WlanConnecting(const char* text, int timeout) {
  int dotCount = 0;
  const int dotSpacing = 6; // Space between dots
  const int dotSize = 8;    // Size of each dot (width and height)

  for (int i = 0; i < 12; i++) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // Get text bounds
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    // Center the text
    int16_t textX = (SCREEN_WIDTH - w) / 2;
    int16_t textY = (SCREEN_HEIGHT - h) / 2 - 10;

    display.setCursor(textX, textY);
    display.println(text);

    // Calculate the total width of the dots including spacing
    int totalDotsWidth = dotSize * 3 + dotSpacing * 2;

    // Center the dots below the text
    int16_t dotsX = (SCREEN_WIDTH - totalDotsWidth) / 2;
    int16_t dotsY = textY + h + 10; // Position below the text

    // Display dots
    display.setTextSize(2); // Increase text size for dots
    for (int j = 0; j < 3; j++) {
      display.setCursor(dotsX + j * (dotSize + dotSpacing), dotsY);
      if (j < dotCount) {
        display.print(".");
      } else {
        display.print(" "); // Print space if dot is not active
      }
    }

    display.display();
    dotCount = (dotCount + 1) % 4;
    delay(200);
  }
}

void WlanConnectionFailed(){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds("Connection failed", 0, 0, &x1, &y1, &w, &h);

    int16_t x = (SCREEN_WIDTH - w) / 2;
    int16_t y = (SCREEN_HEIGHT - h) / 2 - 10;

    display.setCursor(x, y);
    display.println("Connection failed");
}


void MqttDisconnect(const char* text) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    int16_t x = (SCREEN_WIDTH - w) / 2;
    int16_t y = (SCREEN_HEIGHT - h) / 2 - 10;

    display.setCursor(x, y);
    display.println(text);
    
}

void turnOffDisplay(){
  display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void turnONDisplay(){
  display.ssd1306_command(SSD1306_DISPLAYON);
}

void updateDisplay(){
  display.display();
}

void drawLinesAndIcon(bool alarm) {
  // Draw the battery icon in the right-hand corner
  const int batteryIconWidth = 16;  // Width of the battery icon
  const int batteryIconHeight = 8;  // Height of the battery icon
  display.drawBitmap(SCREEN_WIDTH - batteryIconWidth - 2, 2, fullBatteryIcon, batteryIconWidth, batteryIconHeight, WHITE);

  const int soundIconWidth = 16;
  const int soundIconHeight = 16;
  if(!alarm)display.drawBitmap(4, 4, soundAus, soundIconWidth, soundIconHeight, WHITE);
  else display.drawBitmap(4, 4, soundAn, soundIconWidth, soundIconHeight, WHITE);
  
  
  // Draw the top line
  display.drawLine(soundIconWidth + 12, batteryIconHeight + 4, SCREEN_WIDTH, batteryIconHeight + 4, WHITE);
  // Draw the bottom line
  display.drawLine(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH, SCREEN_HEIGHT - 10, WHITE);
}
