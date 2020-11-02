//
// SPI master speedtest - time SPI master connected to a slave, and check if it is reliable
//
//  this transfers a set of bytes from the slave to the master.
//  it times the total transfer, checks the bytes, and repeats every 3 seconds.
//  importantly, the transfer is initiated by the master.
//
// Platform:
//      AVR-based Arduinos only
//
// -Rolf (Oct 2020)
//
// Credits:
//  this code is inspired by an excellent post on the Arduino forum by user GolamMostafa:
//      https://forum.arduino.cc/index.php?topic=66998.0
//

#include <SPI.h>

// "transfer active" LED connected to pin 6
#define LED 6


// sent by master to indicate the beginning of a transfer (from slave to master)
#define MAGIC_BYTE 0xf0

// #bytes to "read" from slave (make sure slave code uses the same defines!)
#define NUM_BYTES 20


// this expects a pattern starting with 0x59, decrementing by 1 for each additional byte.
// one quirk: the transition from 0x50 to 0x49 is done as one step!
//
int chk_pattern(uint8_t *buf, int count)
{
  byte val = 0x59;
  for (byte i = 0; i < count; i++) {
    if (buf[i] != val) {
      return 0; // failed
    }
    if (val == 0x50)
      val = 0x49;
    else
      val--;
  }
  return 1; // ok
}


void setup()
{
  Serial.begin(115200);
  
  pinMode(LED, OUTPUT);                 // LED output
  
  SPI.begin();                          // init SPI library
  SPI.setClockDivider(SPI_CLOCK_DIV8);  // set clock for SPI communication (DIV8 ==> 16/8 = 2Mhz)
  digitalWrite(SS, HIGH);               // initially HIGH, we will set low when we start

  Serial.println("clockDivider: DIV8"); // should match above code
}


void loop()
{
  static byte vals[NUM_BYTES];
  byte rx ,tx;
  int i;
  unsigned long start1, interval; 
  static unsigned long lower = 0xffffffff, upper = 0x0;

  static int xferCount; // #SPI transactions (each being some fixed number of bytes)
  static int failCount; // #failed transactions
  boolean failed;


  digitalWrite(LED, HIGH);          // indicate transfer begin

  start1 = micros();                // start time
  
  tx = MAGIC_BYTE;
  digitalWrite(SS, LOW);
  rx = SPI.transfer(tx);        // send byte to slave, but we also receive a byte (throwaway 1st one)
  digitalWrite(SS, HIGH);

  for (i = 0; i < NUM_BYTES; i++) {     // now read bytes
    digitalWrite(SS, LOW);              // these LOW/HIGH calls were necessary for reliable xfers
    vals[i] = SPI.transfer(0);
    digitalWrite(SS, HIGH);
  }

  interval = micros() - start1;     // end time
  if (interval > upper) {
    upper = interval;
  }
  if (interval < lower) {
    lower = interval;
  }

  xferCount++;

  failed = !chk_pattern(vals, NUM_BYTES);

  digitalWrite(LED, LOW);       // indicate transfer end

  // expecting hex bytes: 59 58 57 ...
  Serial.println("master: bytes received (hex):");
  for (i = 0; i < NUM_BYTES; i++) {
    Serial.print(vals[i], HEX); Serial.print(" ");
  }
  if (failed) {
    Serial.print("- FAILED!");
    failCount++;
  }
  Serial.println("");

  Serial.print("  timing: interval,upper,lower (uS), fails/xfers: ");
  Serial.print(interval); Serial.print(",");
  Serial.print(upper); Serial.print(",");
  Serial.print(lower); Serial.print(",");
  Serial.print(" "); Serial.print(failCount); Serial.print("/");Serial.print(xferCount);
  if (failCount > 0) {
    Serial.print(" F! ");
  }
  Serial.println("");

  delay(3000);
}
