#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <RTClib.h>

// TFT Pins for Arduino Uno
#define cs   10
#define dc   9
#define rst  8

// Define the LED pins
#define LED_A 2  // High demand LED
#define LED_B 3  // Medium demand LED
#define LED_C 4  // Low demand LED

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
RTC_DS3231 rtc;

const char* csvData = "\
Time,Price\n\
1,300\n\
0,280\n\
2,260\n\
3,240\n\
4,220\n\
5,200\n\
6,250\n\
7,300\n\
8,400\n\
9,500\n\
10,600\n\
11,550\n\
12,450\n\
13,450\n\
14,400\n\
15,350\n\
16,300\n\
17,450\n\
18,500\n\
19,600\n\
20,500\n\
21,400\n\
22,350\n\
23,300";

void setup() {
  Serial.begin(9600);

  // Initialize TFT
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

 //rtc.adjust(DateTime(2025, 4, 21, 22, 48, 0));  // Year, Month, Day, Hour, Min, Sec

  // Setup LED pins
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);
}

void loop() {
  DateTime now = rtc.now();
  displayTimeWeatherAndCheckPrice(now);
  delay(1000); // Update every second
}

void displayTimeWeatherAndCheckPrice(DateTime now) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(now.hour(), DEC);
  tft.print(':');
  if (now.minute() < 10) tft.print('0');
  tft.print(now.minute(), DEC);
  tft.print(':');
  if (now.second() < 10) tft.print('0');
  tft.println(now.second(), DEC);

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN);

  if (now.hour() >= 0 && now.hour() < 7) {
    tft.print("Weather: Cold");
  } else if (now.hour() >= 7 && now.hour() < 15) {
    tft.print("Weather: Hot");
  } else if (now.hour() >= 15 && now.hour() < 20) {
    tft.print("Weather: Rainy");
  } else {
    tft.print("Weather: Warm");
  }

  checkElectricityPrice(now);
}

void checkElectricityPrice(DateTime now) {
  int hour = now.hour();
  char searchStr[4];
  sprintf(searchStr, "%d,", hour); // Match "hour," format exactly

  char* priceStr = strstr(csvData, searchStr);
  if (priceStr != NULL) {
    int priceInCents = atoi(strchr(priceStr, ',') + 1);
    float priceInDollars = static_cast<float>(priceInCents) / 100.0;

    if (priceInDollars < 3.99) {
      digitalWrite(LED_C, HIGH);
      digitalWrite(LED_B, LOW);
      digitalWrite(LED_A, LOW);
      tft.setTextColor(ST77XX_GREEN);
      tft.setCursor(0, 50);
      tft.print("Low price: $");
      tft.println(priceInDollars);
      tft.println("Use all appliances.");
    } else if (priceInDollars < 4.99) {
      digitalWrite(LED_B, HIGH);
      digitalWrite(LED_C, LOW);
      digitalWrite(LED_A, LOW);
      tft.setTextColor(ST77XX_YELLOW);
      tft.setCursor(0, 50);
      tft.print("Mid price: $");
      tft.println(priceInDollars);
      tft.println("Use priority appliances.");
    } else {
      digitalWrite(LED_A, HIGH);
      digitalWrite(LED_B, LOW);
      digitalWrite(LED_C, LOW);
      tft.setTextColor(ST77XX_RED);
      tft.setCursor(0, 50);
      tft.print("High price: $");
      tft.println(priceInDollars);
      tft.println("Turn off appliances!");
    }
  }
}
