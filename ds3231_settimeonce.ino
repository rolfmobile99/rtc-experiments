//
// ds3231_settimeonce.ino - set time on a DS3231 RTC module
//
//  -Rolf (Nov 2020)
//
// Use this sketch to "set" the RTC module, 
//  by setting a time and pulling a wire at just the right moment!
//
// Hardware Setup:
// connect the RTC module to the Arduino using SDA and SCL wires.
//
// then, digital pin 10 is pulled up by a 10k resistor, but with a wire also setting the pin to GND.
// at the designated time, you should pull out the "ground" wire. This will set the pin HIGH.
// and it will set the time.
//
// Note: the time needs to be set in the code below. 
// you can pick a time a few minutes in the future, download the code,
// and then watch an accurate clock for the right moment to pull the wire!
//
// Credits:
//  based on original sketch "ds3231" from Adafruit's RTClib
//


#include <stdio.h>
#include "Wire.h"

#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  Serial.begin(115200);
  pinMode(10, INPUT);   // use pin 10 to trigger "set time" action

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // as soon as this pin goes high (ie. by pulling out a ground wire), we set the time to a known value
  if (digitalRead(10) != HIGH) {
    Serial.println("waiting... (for pin 10 to be ungrounded)");
    while (digitalRead(10) == LOW)
      ;
    rtc.adjust(DateTime(2020, 11, 3, 8, 8, 0));   // just do this once - set for a few mins in the future before downloading
    Serial.println("time set!");
  }

}

void loop () {
    char buf[16];

    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    sprintf(buf, "%02d", now.minute());  // make minutes always be 2 digits
    Serial.print(buf);
    Serial.print(':');
    sprintf(buf, "%02d", now.second());  // make seconds always be 2 digits
    Serial.print(buf);
    Serial.println();

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
    delay(3000);
}
