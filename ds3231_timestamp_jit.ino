/*
*  this script sends the time every 2 seconds.
*  it polls the DS3231 to see when the "seconds" register changes, so that the time
*  is as "timely" as possible.
*
*  HARDWARE:
*   Feather M0 (not tested on other Arduinos)
*   Adafruit DS3231 RTC module
*   simply connect SDA to SDA and SCL to SCL, from RTC to the Feather, respectively.
*   make sure the RTC module is powered from the Feather. (power and ground)
* 
*  AUTHOR: Rolf Widenfelt (Nov 2020)
*  - port to DS3231 RTC
*  - add "just in time" code using low-level Wire library
*
*  CREDITS:
*   partially based on original sketch (timestamp for DS1307) by AxelTB 2015-06-01
*  
*  RELATED:
*   this works together with a host script that receives "full" timestamp: "2020-11-07T09:25:57"
*   and compares it to host system time.  It expects the RTC to be UTC time.
*   (see related script rtc_read.py)
*  
*/

#include <stdio.h>
#include <Arduino.h>

// Macro to deal with the difference in I2C write functions from old and new
// Arduino versions.
#define _I2C_WRITE write ///< Modern I2C write
#define _I2C_READ read   ///< Modern I2C read

#include <Wire.h>

#define DS3231_ADDRESS 0x68

// forward refs
int rtc_init();
int rtc_read(uint8_t *buf, unsigned long *interval);
int rtc_read1(uint8_t *buf);
int rtc_wait();


#include "RTClib.h"


RTC_DS3231 rtc;


void setup ()
{
    Serial.begin(115200);
    Serial.flush();

    Wire.begin();           // initialize "Wire" library (needed!)
    Wire.setClock(400000);  // this greatly increases the speed!

    Serial.println("Initializing Wire...");
    delay(500);

    if (rtc_init() == 0) {
        Serial.println("Oops. No RTC");
        Serial.flush();
        abort();
    }

    if (!rtc.begin()) {
        Serial.println("Oops. No RTC (2)");
        Serial.flush();
        abort();
    }
}


void loop()
{

    rtc_wait();   // waits for "seconds" register in DS3231 to change
    
    DateTime time = rtc.now();
    
    // print full timestamp prefixed with "rtc:"
    Serial.println(String("rtc: ")+time.timestamp(DateTime::TIMESTAMP_FULL));
    
    // delay a specific amount (just over 1 sec to "force" a 2-second cycle)
    delay(1400);
}


int rtc_init()
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire._I2C_WRITE((uint8_t)0);
    if (Wire.endTransmission() != 0)
        return 0;   // failed!
    else
        return 1;
}


int rtc_read1(uint8_t *buf)
{
    int i = 0;

    // Note: Q. Is this begin/end thing needed every time? Yes, needed.

    Wire.beginTransmission(DS3231_ADDRESS);
    Wire._I2C_WRITE((uint8_t)0);
    if (Wire.endTransmission() != 0)
        return 0;   // failed!


    Wire.requestFrom(DS3231_ADDRESS, 1);
    buf[i++] = Wire._I2C_READ() & 0x7F;
#ifdef NOTDEF
    buf[i++] = Wire._I2C_READ();
    buf[i++] = Wire._I2C_READ();
    Wire._I2C_READ();
    buf[i++] = Wire._I2C_READ();
    buf[i++] = Wire._I2C_READ();
    buf[i++] = Wire._I2C_READ();
#endif

    return 1;
}


//
// waits for a change in the "seconds" register of the DS3231
//
int rtc_wait()
{
    uint8_t buf[4];
    uint8_t prev_byte;
    int changed = 0;
    
    (void)rtc_read1(buf);  // read first time
    prev_byte = buf[0];
    
    while (!changed) {
      if (rtc_read1(buf) == 0)   // error condition (shouldn't happen)
        return 0; // error
    
        if (buf[0] != prev_byte)
          changed = 1;
    }
    return 1; // ok
}

