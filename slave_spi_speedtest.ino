//
// SPI slave - SPI slave speedtest
//
//  (see SPI master speedtest for details)
//
// -Rolf (Oct 2020)
//
// Credits:
//  the SPI code below is inspired by an excellent post on the Arduino forum by user GolamMostafa:
//      https://forum.arduino.cc/index.php?topic=66998.0
//

#include <stdio.h>
#include <SPI.h>
#include <Arduino.h>


#define MAGIC_BYTE 0xf0

#define NUM_BYTES 20


volatile boolean receivedFlag;

// there are two buffers - for this code, we only use the "A" buffer

volatile byte valsA[NUM_BYTES], valsB[NUM_BYTES], *vals_p;
volatile int valCount;
volatile int valIndex;
volatile boolean val_A_flag;


void setup()
{
    Serial.begin(115200);
    
    //SPI.begin();              // note: do not call this on the "slave" side

    pinMode(SS, INPUT_PULLUP);
    pinMode(MISO, OUTPUT);                  // Sets MISO as OUTPUT (Slave Out)
    pinMode(SCK, INPUT);

    SPCR |= _BV(SPE);                       // Turn on SPI in Slave Mode (AVR-only hack)

    receivedFlag = false;

    Serial.println("enable SPI interrupt");

    SPI.attachInterrupt();                  // interrupt is set for SPI communication

#ifdef NOTDEF
    byte r = SPCR;
    Serial.print("SPCR: (hex) "); Serial.println(r, HEX);  // typically 0xC0 for slave
#endif

    // now setup data buffer

    valIndex = 0;

    vals_p = valsA; // initially we point to the "A" buffer
    val_A_flag = true;

    // load current buffer with known data to send to master (0x59 0x58 0x57 ... )
    byte val = 0x59;
    for (int i = 0; i < NUM_BYTES; i++) {
        vals_p[i] = val;
        if (val == 0x50) {
            val = 0x49;
        } else {
            val--;
        }
    }

    valCount = NUM_BYTES;       // set byte count
}


void loop()
{
    if (receivedFlag) {
        receivedFlag = false;

        /* maybe do something interesting here? */
    }

    delay(10);     // XXX needed??
}


ISR (SPI_STC_vect)          // interrupt handler (SPI serial transfer complete)
{
    byte rx, tx;

    rx = SPDR;              // receive a byte from master
    receivedFlag = true;    // Sets received as True

    if (rx == MAGIC_BYTE) {     // detect magic byte, which indicates "start" of transfer
        valIndex = 0;
    }

    if (valIndex < valCount) {
        tx = vals_p[valIndex++];    // access current buffer via pointer
    } else {
        tx = 0;
    }

    SPDR = tx;  // return (send) a byte to master via SPDR register
}
