// SPI slave - simple demo - slave side
//
// -Rolf (Oct 2020)
//
// Credits:
//  this code is largely inspired by an excellent post on the Arduino forum by user GolamMostafa:
//      https://forum.arduino.cc/index.php?topic=66998.0
//

#include <SPI.h>

#define LED 6

volatile boolean receivedFlag;

volatile byte vals[16];
volatile int valCount;
volatile int valIndex;


#define MAGIC_BYTE 0xf0


void setup()
{
    Serial.begin(115200);

    pinMode(LED, OUTPUT);                    // LED output

    valIndex = 0;

    vals[0] = 0x59;     // dummy data we'd like to send (to master)
    vals[1] = 0x43;
    vals[2] = 0x16;
    vals[3] = 0x15;
    vals[4] = 0x14;
    valCount = 5;       // byte count

    //SPI.begin();              // note: do not call this on the "slave" side

    pinMode(SS, INPUT_PULLUP);
    pinMode(MISO, OUTPUT);                  // Sets MISO as OUTPUT (Slave Out)
    pinMode(SCK, INPUT);

    SPCR |= _BV(SPE);                       // Turn on SPI in Slave Mode (AVR-only hack)

    receivedFlag = false;

    Serial.println("enable SPI interrupt");

    SPI.attachInterrupt();                  // interrupt is set for SPI communication
}


void loop()
{
    static int count = 0;

    if (receivedFlag) {
        receivedFlag = false;

        Serial.print(".");          // debug - print a "." for each byte transferred
        if (++count == 30) {
            Serial.println("");
            count = 0;
        }
    }
}


ISR (SPI_STC_vect)          // interrupt handler 
{
    byte rx, tx;

    rx = SPDR;              // receive a byte from master
    receivedFlag = true;    // Sets received as True

    if (rx == MAGIC_BYTE) {     // detect magic byte, which indicates "start" of transfer
        valIndex = 0;
    }

    if (valIndex < valCount) {
        tx = vals[valIndex++];
    } else {
        tx = 0;
    }

    SPDR = tx;  // return (send) a byte to master via SPDR register
}
