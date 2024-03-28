#include <Wire.h>
#include <RTClib.h>

#define LED_PIN 8
#define LED_PIN2 6
#define LED_PIN3 5

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
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
}

void loop() {
  DateTime now = rtc.now();

  // Check electricity price
  checkElectricityPrice(now);

  delay(1000); // Update every 1 second
}

void checkElectricityPrice(DateTime now) {
  int hour = now.hour();

  char searchStr[4];
  sprintf(searchStr, "%02d", hour);

  char* priceStr = strstr(csvData, searchStr);

  if (priceStr != NULL) {
    int priceInCents = atoi(strchr(priceStr, ',') + 1);
    float priceInDollars = static_cast<float>(priceInCents) / 100.0;

    if (priceInDollars < 3.99) {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(LED_PIN2, LOW);
      digitalWrite(LED_PIN3, LOW);
      Serial.print("\nLow price: $");
      Serial.println(priceInDollars);
      Serial.println("\nadvisable to switch on all appliances");
    } else if (priceInDollars < 4.99) {
      digitalWrite(LED_PIN2, HIGH);
      digitalWrite(LED_PIN3, LOW);
      digitalWrite(LED_PIN, LOW);
      Serial.print("\nMid price: $");
      Serial.println(priceInDollars);
      Serial.println("\nadvisable to switch on priority appliances");
    } else {
      digitalWrite(LED_PIN3, HIGH);
      digitalWrite(LED_PIN2, LOW);
      digitalWrite(LED_PIN, LOW);
      Serial.print("\nHigh price: $");
      Serial.println(priceInDollars);
      Serial.println("\nadvisable to turn off all appliances");
    }
  }
}
