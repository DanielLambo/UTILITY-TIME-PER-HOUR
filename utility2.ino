#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <RTClib.h>

#define TFT_CS 53
#define TFT_RST 48
#define TFT_DC 49
#define LED_PIN    7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
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
17,400\n\
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

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  DateTime now = rtc.now();

  // Display the time, weather, and check price
  displayTimeWeatherAndCheckPrice(now);

  delay(1000); // Update every 1 second
}

void displayTimeWeatherAndCheckPrice(DateTime now) {
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);

  // Print the time
  tft.setTextSize(2);
  tft.setTextColor(ST7735_WHITE);
  tft.print(now.hour(), DEC);
  tft.print(':');
  if (now.minute() < 10) {
    tft.print('0');
  }
  tft.print(now.minute(), DEC);
  tft.print(':');
  if (now.second() < 10) {
    tft.print('0');
  }
  tft.println(now.second(), DEC);

  // Print the weather based on the time
  tft.setTextSize(1);
  tft.setTextColor(ST7735_CYAN);

  if (now.hour() >= 0 && now.hour() < 7) {
    tft.print("Weather: Cold");
  } else if (now.hour() >= 7 && now.hour() < 15) {
    tft.print("Weather: Hot");
  } else if (now.hour() >= 15 && now.hour() < 20) {
    tft.print("Weather: Rainy");
  } else {
    tft.print("Weather: Warm");
  }

  // Check electricity price
  checkElectricityPrice(now);
}

void checkElectricityPrice(DateTime now) {
  int hour = now.hour();

  char searchStr[4];
  sprintf(searchStr, "%02d", hour);

  char* priceStr = strstr(csvData, searchStr);

  if (priceStr != NULL) {
    int priceInCents = atoi(strchr(priceStr, ',') + 1);
    float priceInDollars = static_cast<float>(priceInCents) / 100.0;

    if (priceInDollars < 5.01) {
      digitalWrite(LED_PIN, HIGH);
      tft.setTextSize(1);
      tft.setTextColor(ST7735_GREEN);
      tft.print("\nLow price: $");
      tft.println(priceInDollars);
    } else {
      digitalWrite(LED_PIN, LOW);
      tft.setTextSize(1);
      tft.setTextColor(ST7735_RED);
      tft.print("\nHigh price: $");
      tft.println(priceInDollars);
    }
  }
}
