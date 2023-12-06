#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <RTClib.h>

#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9
#define LED_PIN    7  // Connect the LED to this pin

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

  // Display the time on TFT and check price
  displayTimeAndCheckPrice(now);

  delay(1000); // Update every 1 second
}

void displayTimeAndCheckPrice(DateTime now) {
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

  // Check electricity price
  checkElectricityPrice(now);
}

void checkElectricityPrice(DateTime now) {
  int hour = now.hour();

  // Find the corresponding row in CSV data
  char searchStr[4];
  sprintf(searchStr, "%02d", hour);

  char* priceStr = strstr(csvData, searchStr);

  if (priceStr != NULL) {
    // Extract the price value in cents
    int priceInCents = atoi(priceStr + strlen(searchStr) + 1);

    // Convert cents to dollars
    float priceInDollars = static_cast<float>(priceInCents);

    if (priceInDollars < 501.0) {
      // Low price, turn on LED and display message
      digitalWrite(LED_PIN, HIGH);
      tft.setTextSize(1);
      tft.setTextColor(ST7735_GREEN);
      tft.print("Low price: $");
      tft.println(priceInDollars);
    } else {
      // High price, turn off LED and display message
      digitalWrite(LED_PIN, LOW);
      tft.setTextSize(1);
      tft.setTextColor(ST7735_GREEN);
      tft.print("High price: $");
      tft.println(priceInDollars);
    }}}
