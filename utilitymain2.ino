#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

// Define the LED pins
#define LED_A 2  // High demand LED
#define LED_B 7  // Medium demand LED
#define LED_C 4  // Low demand LED

LiquidCrystal_I2C lcd(0x3F, 16, 2); // I2C address may vary
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

  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

//rtc.adjust(DateTime(2025, 4, 21, 23, 41, 0));  // Set manually once if needed

  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);

  Serial.println("System Initialized.");
}

void loop() {
  DateTime now = rtc.now();
  displayTimeAndPrice(now);
  delay(1000); // Update every second
}


void displayTimeAndPrice(DateTime now) {
  lcd.clear();

  // Time
  char timeBuffer[10];
  sprintf(timeBuffer, "%02d:%02d", now.hour(), now.minute());

  lcd.setCursor(0, 0);
  lcd.print("Time ");
  lcd.print(timeBuffer);

  Serial.print("Current Time: ");
  Serial.println(timeBuffer);

  // Pricing
  checkElectricityPrice(now);
}

void checkElectricityPrice(DateTime now) {
  int hour = now.hour();
  char searchStr[4];
  sprintf(searchStr, "%d,", hour);

  char* priceStr = strstr(csvData, searchStr);
  if (priceStr != NULL) {
    int priceInCents = atoi(strchr(priceStr, ',') + 1);
    float priceInDollars = static_cast<float>(priceInCents) / 100.0;

    lcd.setCursor(0, 1); // Line 2
    Serial.print("Price: $");
    Serial.println(priceInDollars, 2);

    if (priceInDollars < 3.99) {
      digitalWrite(LED_C, HIGH);
      digitalWrite(LED_B, HIGH);
      digitalWrite(LED_A, LOW);
      lcd.print("Low: $");
      lcd.print(priceInDollars, 2);
      Serial.println("Advice: Use all appliances");
    } else if (priceInDollars < 4.99) {
      digitalWrite(LED_B, HIGH);
      digitalWrite(LED_C, LOW);
      digitalWrite(LED_A, LOW);
      lcd.print("Med: $");
      lcd.print(priceInDollars, 2);
      Serial.println("Advice: Use priority appliances");
    } else {
      digitalWrite(LED_A, HIGH);
      digitalWrite(LED_B, LOW);
      digitalWrite(LED_C, LOW);
      lcd.print("High: $");
      lcd.print(priceInDollars, 2);
      Serial.println("Advice: Turn off appliances");
    }

    Serial.println("--------------------------");
  } else {
    Serial.println("Price not found for this hour.");
  }
}
