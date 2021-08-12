// This example demonstrates how to use 2 MCP23S17 chips.

#include <MCP23S17.h>

#ifdef __PIC32MX__
// chipKIT uses the DSPI library instead of the SPI library as it's better
#include <DSPI.h>
DSPI0 SPI;
#else
// Everytying else uses the SPI library
#include <SPI.h>
#endif

const uint8_t targetPin = 15;
const uint8_t chipSelect = 10;

// Create an object for each chip
// Bank1 uses address 0: Pins A0,A1,A2 ground.
// Bank2 uses address 1: Pin A0=VCC, A1,A2 ground.
// Increase the addresses by 2 for each BA value.
MCP23S17 Bank1(&SPI, chipSelect, 0);
MCP23S17 Bank2(&SPI, chipSelect, 1);

void setup() {
    Bank1.begin();
    Bank2.begin();

    Bank1.pinMode(targetPin, OUTPUT);
    Bank2.pinMode(targetPin, INPUT_PULLUP);
}

void loop() {
    Bank1.digitalWrite(targetPin, !Bank2.digitalRead(targetPin));
}
