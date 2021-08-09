#include <Arduino.h>
#include <MCP23S17.h>
#include <SPI.h>

const uint8_t address = 0;
const uint8_t interruptPinA = 2;
const uint8_t chipSelectPin = 10;

volatile uint16_t mcpReading = 0; // 16-bit interrupt reading
volatile uint8_t mcpAReading = 0; // 8-bit Port-A interrupt reading
volatile uint8_t mcpBReading = 0; // 8-bit Port-B interrupt reading
volatile uint8_t flagMCPA = 0;
volatile uint8_t flagINTA = 0;
volatile uint8_t flagMCPB = 0;
volatile uint8_t flagINTB = 0;
uint8_t whichBit = 0;    // one bit [7...0] that is set in byte
uint8_t byteACount = 0; // number of bits set in byte
uint8_t byteBCount = 0; // number of bits set in byte
uint8_t byteABits[8] = {0}; // set bit array
uint8_t byteBBits[8] = {0}; // set bit array

MCP23S17 mcp23s17(&SPI, chipSelectPin, address);

void print8Bits(uint8_t myByte) {
  for (uint8_t mask = 0x80; mask; mask >>= 1) {
    if (mask  & myByte)
      Serial.print('1');
    else
      Serial.print('0');
  }
}

//---------------------------------------------------------------------------------
// print 16-bit word as 16 bit binary string
//---------------------------------------------------------------------------------

void print16Bits(uint16_t myWord) {
  for (uint16_t mask = 0x8000; mask; mask >>= 1) {
    if (mask & myWord)
      Serial.print('1');
    else
      Serial.print('0');
  }
}


//---------------------------------------------------------------------------------
// crPrintHEX print value as hex with specified number of digits
//---------------------------------------------------------------------------------

void crPrintHEX(unsigned long DATA, unsigned char numChars) {
  unsigned long mask  = 0x0000000F;
  mask = mask << 4 * (numChars - 1);
  Serial.print("0x");
  for (unsigned int eID = numChars; eID > 0;  --eID) {
    Serial.print(((DATA & mask) >> (eID - 1) * 4), HEX);
    mask = mask >> 4;
  }
  Serial.print("  ");
}

// Set all Port Expander pins to desired mode INPUT_PULLUP
// Set all Port Expander input pins interrupts
void setPin(MCP23S17 &bank) {
  for (uint8_t ind = 0; ind <= 15; ind++) {
    bank.pinMode(ind, INPUT_PULLUP);
    bank.enableInterrupt(ind, FALLING);
  }

  // set Port Expander Interrupt configuratons
  bank.setMirror(false);
  bank.setInterruptOD(false);
  bank.setInterruptLevel(LOW);

  // clear all interrupts on this Port Expander
  mcpReading = bank.getInterruptValue();
}

// Set all Chip pins to desired mode
void setChipPins() {
  // Example: pass the Bank object to the setPin function

  setPin(mcp23s17);
  /*setPin(Bank1);
  setPin(Bank2);
  setPin(Bank3);*/
}

// Read all Port Expander port interrupt pins
// save set bit to byteBits arrays
// byteBits array can be sequentially processed later
void readByteBits(MCP23S17 &bank, uint8_t port) {
  if (port < 1) { // Port A
    byteACount = 0;
    for (uint8_t ind = 0; ind < 8; ind++) {
      if (bitRead(~mcpAReading, ind)) { // ~ inverts bit values
        byteABits[byteACount] = ind; // save the bit number to sequential array
        byteACount = byteACount + 1;
        Serial.print ("\n Port A whichBit: "); Serial.print (ind);
      }
    }
    //Serial.print ("\n byteACount: "); Serial.print (byteACount);
  } else { // Port B
    byteBCount = 0;
    for (uint8_t ind = 0; ind < 8; ind++) {
      if (bitRead(~mcpBReading, ind)) { // ~ inverts bit values
        byteBBits[byteBCount] = ind; // save the bit number to sequential array
        byteBCount = byteBCount + 1;
        Serial.print ("\n Port B whichBit: "); Serial.print (ind);
      }
    }
    //Serial.print ("\n byteBCount: "); Serial.print (byteBCount);
  }
}

// Read all Port Expander port interrupt values
void readBankPortIntVal(MCP23S17 &bank, uint8_t port) {
  if (port < 1) { // Port A
    mcpAReading = bank.getInterruptAValue();
    //Serial.print ("\n Port A Interrupt Values: "); print8Bits(mcpAReading);
  } else {        // Port B
    mcpBReading = bank.getInterruptBValue();
    //Serial.print ("\n Port B Interrupt Values: "); print8Bits(mcpBReading);
  }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++
// function readBankPortIntPins()
// ++++++++++++++++++++++++++++++++++++++++++++++++++++

// Read all Port Expander port interrupt pins
void readBankPortIntPins(MCP23S17 &bank, uint8_t port) {
  if (port < 1) { // Port A
    mcpAReading = bank.getInterruptAPins();
    //Serial.print ("\n Port A Interrupt Pins: "); print8Bits(mcpAReading);
  } else {        // Port B
    mcpBReading = bank.getInterruptBPins();
    //Serial.print ("\n Port B Interrupt Pins: "); print8Bits(mcpBReading);
  }
}

void interruptMCP(MCP23S17 &bank, uint8_t port) {
  // read the Port Expander Interrupt pins
  readBankPortIntPins(bank, port);

  // read the Port Expander Interrupt values
  readBankPortIntVal(bank, port);

  // take action on new pin interrupt values
  if (port < 1) {
    flagMCPA = 1; // set flag that Port A interrupt occurred
  } else {
    flagMCPB = 1; // set flag that Port B interrupt occurred
  }
  // act on Reading in loop()
}

void printInterruptHeader(MCP23S17 bank, uint8_t port) {
  // inReading is either mcpAReading or mcbBReading 8-pin byte values
  /*Serial.print("\n\n --- Interrupts Port ");
  if (port < 1) {
    Serial.print(F("A "));
    Serial.print("Values: "); print8Bits(~mcpAReading);
  } else {
    Serial.print(F("B "));
    Serial.print("Values: "); print8Bits(~mcpBReading);

  }
  */
  // get the bit count and write the bits to byteBit[] array
  readByteBits(bank, port);
  Serial.println();
}

void interruptA() {
  // ISR to respond to INT0 interrupt
  // Respond to the INTA interrupt
  // which was triggered by one or more MCP inputs
  interruptMCP(mcp23s17, 0); // MCP ISR (bank, port)
}

// Process the port byteBits array of interrupted MCP pins
void processPins(MCP23S17 &bank, uint8_t port) {
  if (port < 1) { // Port A
    // byteACount = number of pins to process
    for (uint8_t ind = 0; ind < byteACount; ind++) {
      uint8_t pin = byteABits[ind]; // recover the bit number from sequential array
      Serial.print("Process port A pin: ");
      Serial.println(pin);
    }
  } else { // Port B
    // byteACount = number of pins to process
    for (uint8_t ind = 0; ind < byteBCount; ind++) {
      uint8_t pin = byteBBits[ind]; // recover the bit number from sequential array
      Serial.print("Process port B pin: ");
      Serial.println(pin);
    }
  }
}


void setup() {
  mcp23s17.begin();
  /*Bank1.begin();
  Bank2.begin();
  Bank3.begin();*/

  pinMode(interruptPinA, INPUT_PULLUP);
  //pinMode(pinB, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interruptPinA), interruptA, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(pinB), interruptB, CHANGE);

  Serial.begin(115200);
  Serial.println(F("Starting MCP23S17 Interrupt Test"));

  //displayInstructions();
  //displayReady();

  //----------------------------------------------------------
  // Port Expander pin and interrupts configuration
  //----------------------------------------------------------
  //
  // Input port code
  // pins 0-15 are on device:port
  // device==1 Bank0
  // port==0 Port A, 1 Port B
  //
  // Set MCP23S17 pin modes and Interrupt configurations
  // example set one pin: chip.pinMode(0, INPUT_PULLUP);
  //
  setChipPins(); // use loop to set individual pins

  // clear the interrupt values variables
  mcpAReading = 0;
  mcpBReading = 0;
  mcpReading = 0;
}

void loop() {
  // This code runs the MCP Interrupt processing from the Arduino ISR
  // and then takes action (prints) on the MCP pins that went LOW

  if (flagMCPA > 0) {
    //Serial.println("loop flagMCPA");
    printInterruptHeader(mcp23s17, 0);
    processPins(mcp23s17, 0); // do some processing of each interrupted pin
    flagMCPA = 0;
  }

  if (flagMCPB > 0) {
    //Serial.println("loop flagMCPB");
    printInterruptHeader(mcp23s17, 1);
    processPins(mcp23s17, 1); // do some processing of each interrupted pin
    flagMCPB = 0;
  }

  //delay(10); // give loop something to do while idle
}
