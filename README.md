# rtc-experiments

## Simple demo of SPI communication between two Arduino boards.

There are two sketches here:
`master_spi.c`
`slave_spi.c`

Each of these runs on its own Arduino.
The master should be wired to the slave with the SPI pins connected one-to-one (SS, MOSI, MISO, SCLK).
There is some debugging output from master, so you might want to open the Serial Monitor to see that.

**Note:** 
It is assumed that these are AVR-based Arduinos (due to some register-hacking).  
(tested with Adafruit Pro Trinket and Arduino Uno)

There next two sketches are:
`master_spi_speedtest.c`
`slave_spi_speedtest.c`

Similar to the above two sketches, each of these should be run on its own Arduino. 
The goal of these sketches is to see how well SPI data transfer works, and what speed can be achieved.
It also verifies the results, since there seem to be infrequent errors, unfortunately.

**Note:**
The stated maximum clock speed for an SPI slave is Fosc/4. This demo is running at Fosc/8.
