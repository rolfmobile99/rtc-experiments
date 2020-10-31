// SPI master - simple demo of an SPI master connected to a slave
//
// -Rolf (Oct 2020)
//
// Credits:
//  this code is largely inspired by an excellent post on the Arduino forum by user GolamMostafa:
//      https://forum.arduino.cc/index.php?topic=66998.0


#include <SPI.h>

// LED connected to pin 6
#define LED 6


// sent by master to indicate the beginning of a transfer (from slave to master)
#define MAGIC_BYTE 0xf0


#if 0
// choices for SPI.setClockDivider() function
#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06
#endif


void setup()
{
  Serial.begin(115200);
  
  pinMode(LED, OUTPUT);                     // LED output
  
  SPI.begin();                              // init SPI library
  //SPI.setClockDivider(SPI_CLOCK_DIV8);    // set clock for SPI communication (16/8 = 2Mhz) - didn't work!
  SPI.setClockDivider(SPI_CLOCK_DIV128);    // set slower clock for SPI communication (128KHz?)
  digitalWrite(SS,HIGH);                    // initially HIGH, we will set low when we start
}


void loop()
{
  static byte vals[16];
  byte rx ,tx;
  int i;         

  digitalWrite(SS, LOW);        // starts communication with Slave connected to master
  
  digitalWrite(LED, HIGH);      // indicate transfer begin

  tx = MAGIC_BYTE;
  rx = SPI.transfer(tx);        // send byte to slave, but we also receive a byte (throwaway)

  for (i = 0; i < 5; i++) {     // transfer additional bytes
    vals[i] = SPI.transfer(0);
  }

  digitalWrite(LED, LOW);       // indicate transfer end

  Serial.println("master: bytes received (hex):");
  for (i = 0; i < 5; i++) {
    Serial.print(vals[i], HEX); Serial.print(" ");
  }
  Serial.println("");

  delay(3000);
}
