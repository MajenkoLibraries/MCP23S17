/*
 * Copyright (c) 2014, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *  1. Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 * 
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 * 
 *  3. Neither the name of Majenko Technologies nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <MCP23S17.h>

/*! The constructor takes three parameters.  The first is an SPI class
 *  pointer.  This is the address of an SPI object (either the default
 *  SPI object on the Arduino, or an object made using the DSPIx classes
 *  on the chipKIT).  The second parameter is the chip select pin number
 *  to use when communicating with the chip.  The third is the internal
 *  address number of the chip.  This is controlled by the three Ax pins
 *  on the chip.
 *  
 *  Example:
 * 
 *  
 *      MCP23S17 myExpander(&SPI, 10, 0);
 * 
 */
#ifdef __PIC32MX__
MCP23S17::MCP23S17(DSPI *spi, uint8_t cs, uint8_t addr) {
#else
MCP23S17::MCP23S17(SPIClass *spi, uint8_t cs, uint8_t addr) {
#endif
    _spi = spi;
    _cs = cs;
    _addr = addr;

    _reg[MCP_IODIRA] = 0xFF;
    _reg[MCP_IODIRB] = 0xFF;
    _reg[MCP_IPOLA] = 0x00;
    _reg[MCP_IPOLB] = 0x00;
    _reg[MCP_GPINTENA] = 0x00;
    _reg[MCP_GPINTENB] = 0x00;
    _reg[MCP_DEFVALA] = 0x00;
    _reg[MCP_DEFVALB] = 0x00;
    _reg[MCP_INTCONA] = 0x00;
    _reg[MCP_INTCONB] = 0x00;
    _reg[MCP_IOCONA] = 0x18;
    _reg[MCP_IOCONB] = 0x18;
    _reg[MCP_GPPUA] = 0x00;
    _reg[MCP_GPPUB] = 0x00;
    _reg[MCP_INTFA] = 0x00;
    _reg[MCP_INTFB] = 0x00;
    _reg[MCP_INTCAPA] = 0x00;
    _reg[MCP_INTCAPB] = 0x00;
    _reg[MCP_GPIOA] = 0x00;
    _reg[MCP_GPIOB] = 0x00;
    _reg[MCP_OLATA] = 0x00;
    _reg[MCP_OLATB] = 0x00;
}

#ifdef __PIC32MX__
MCP23S17::MCP23S17(DSPI &spi, uint8_t cs, uint8_t addr) {
#else
MCP23S17::MCP23S17(SPIClass &spi, uint8_t cs, uint8_t addr) {
#endif
    _spi = &spi;
    _cs = cs;
    _addr = addr;

    _reg[MCP_IODIRA] = 0xFF;
    _reg[MCP_IODIRB] = 0xFF;
    _reg[MCP_IPOLA] = 0x00;
    _reg[MCP_IPOLB] = 0x00;
    _reg[MCP_GPINTENA] = 0x00;
    _reg[MCP_GPINTENB] = 0x00;
    _reg[MCP_DEFVALA] = 0x00;
    _reg[MCP_DEFVALB] = 0x00;
    _reg[MCP_INTCONA] = 0x00;
    _reg[MCP_INTCONB] = 0x00;
    _reg[MCP_IOCONA] = 0x18;
    _reg[MCP_IOCONB] = 0x18;
    _reg[MCP_GPPUA] = 0x00;
    _reg[MCP_GPPUB] = 0x00;
    _reg[MCP_INTFA] = 0x00;
    _reg[MCP_INTFB] = 0x00;
    _reg[MCP_INTCAPA] = 0x00;
    _reg[MCP_INTCAPB] = 0x00;
    _reg[MCP_GPIOA] = 0x00;
    _reg[MCP_GPIOB] = 0x00;
    _reg[MCP_OLATA] = 0x00;
    _reg[MCP_OLATB] = 0x00;
}

/*! The begin function performs the initial configuration of the IO expander chip.
 *  Not only does it set up the SPI communications, but it also configures the chip
 *  for address-based communication and sets the default parameters and registers
 *  to sensible values.
 *
 *  Example:
 *
 *      myExpander.begin();
 *
 */
void MCP23S17::begin() {
    ::pinMode(_cs, OUTPUT);
    ::digitalWrite(_cs, HIGH);
    uint8_t cmd = 0b01000000;
    _spi->beginTransaction(SPI_MCP23S17_SETTINGS);
    ::digitalWrite(_cs, LOW);
    _spi->transfer(cmd);
    _spi->transfer(MCP_IOCONA);
    _spi->transfer(0x18);
    ::digitalWrite(_cs, HIGH);
    _spi->endTransaction();
    writeAll();
}

/*! This private function reads a value from the specified register on the chip and
 *  stores it in the _reg array for later usage.
 */
void MCP23S17::readRegister(uint8_t addr) {
    if (addr > 21) {
        return;
    }
    uint8_t cmd = 0b01000001 | ((_addr & 0b111) << 1);
    _spi->beginTransaction(SPI_MCP23S17_SETTINGS);
    ::digitalWrite(_cs, LOW);
    _spi->transfer(cmd);
    _spi->transfer(addr);
    _reg[addr] = _spi->transfer(0xFF);
    ::digitalWrite(_cs, HIGH);
    _spi->endTransaction();
}

/*! This private function writes the current value of a register (as stored in the
 *  _reg array) out to the register in the chip.
 */
void MCP23S17::writeRegister(uint8_t addr) {
    if (addr > 21) {
        return;
    }
    uint8_t cmd = 0b01000000 | ((_addr & 0b111) << 1);
    _spi->beginTransaction(SPI_MCP23S17_SETTINGS);
    ::digitalWrite(_cs, LOW);
    _spi->transfer(cmd);
    _spi->transfer(addr);
    _spi->transfer(_reg[addr]);
    ::digitalWrite(_cs, HIGH);
    _spi->endTransaction();
}

/*! This private function performs a bulk read on all the registers in the chip to
 *  ensure the _reg array contains all the correct current values.
 */
void MCP23S17::readAll() {
    uint8_t cmd = 0b01000001 | ((_addr & 0b111) << 1);
    _spi->beginTransaction(SPI_MCP23S17_SETTINGS);
    ::digitalWrite(_cs, LOW);
    _spi->transfer(cmd);
    _spi->transfer(0);
    for (uint8_t i = 0; i < 22; i++) {
        _reg[i] = _spi->transfer(0xFF);
    }
    ::digitalWrite(_cs, HIGH);
    _spi->endTransaction();
}

/*! This private function performs a bulk write of all the data in the _reg array
 *  out to all the registers on the chip.  It is mainly used during the initialisation
 *  of the chip.
 */
void MCP23S17::writeAll() {
    uint8_t cmd = 0b01000000 | ((_addr & 0b111) << 1);
    _spi->beginTransaction(SPI_MCP23S17_SETTINGS);
    ::digitalWrite(_cs, LOW);
    _spi->transfer(cmd);
    _spi->transfer(0);
    for (uint8_t i = 0; i < 22; i++) {
        _spi->transfer(_reg[i]);
    }
    ::digitalWrite(_cs, HIGH);
    _spi->endTransaction();
}

/*! Just like the pinMode() function of the Arduino API, this function sets the
 *  direction of the pin.  The first parameter is the pin nimber (0-15) to use,
 *  and the second parameter is the direction of the pin.  There are standard
 *  Arduino macros for different modes which should be used.  The supported macros are:
 *
 *  * OUTPUT
 *  * INPUT
 *  * INPUT_PULLUP
 *
 *  The INPUT_PULLUP mode enables the weak pullup that is available on any pin.
 *
 *  Example:
 *
 *      myExpander.pinMode(5, INPUT_PULLUP);
 */
void MCP23S17::pinMode(uint8_t pin, uint8_t mode) {
    if (pin >= 16) {
        return;
    }
    uint8_t dirReg = MCP_IODIRA;
    uint8_t puReg = MCP_GPPUA;
    if (pin >= 8) {
        pin -= 8;
        dirReg = MCP_IODIRB;
        puReg = MCP_GPPUB;
    }

    switch (mode) {
        case OUTPUT:
            _reg[dirReg] &= ~(1<<pin);
            writeRegister(dirReg);
            break;

        case INPUT:
        case INPUT_PULLUP:
            _reg[dirReg] |= (1<<pin);
            writeRegister(dirReg);
            if (mode == INPUT_PULLUP) {
                _reg[puReg] |= (1<<pin);
            } else {
                _reg[puReg] &= ~(1<<pin);
            }
            writeRegister(puReg);
            break;
    }
}

/*! Like the Arduino API's namesake, this function will set an output pin to a specific
 *  value, either HIGH (1) or LOW (0).  If the pin is currently set to an INPUT instead of
 *  an OUTPUT, then this function acts like the old way of enabling / disabling the pullup
 *  resistor, which pre-1.0.0 versions of the Arduino API used - i.e., set HIGH to enable the
 *  pullup, or LOW to disable it.
 *
 *  Example:
 *
 *      myExpander.digitalWrite(3, HIGH);
 */
void MCP23S17::digitalWrite(uint8_t pin, uint8_t value) {
    if (pin >= 16) {
        return;
    }
    uint8_t dirReg = MCP_IODIRA;
    uint8_t puReg = MCP_GPPUA;
    uint8_t latReg = MCP_OLATA;
    if (pin >= 8) {
        pin -= 8;
        dirReg = MCP_IODIRB;
        puReg = MCP_GPPUB;
        latReg = MCP_OLATB;
    }

    uint8_t mode = (_reg[dirReg] & (1<<pin)) == 0 ? OUTPUT : INPUT;
    
    switch (mode) {
        case OUTPUT:
            if (value == 0) {
                _reg[latReg] &= ~(1<<pin);
            } else {
                _reg[latReg] |= (1<<pin);
            }
            writeRegister(latReg);
            break;

        case INPUT:
            if (value == 0) {
                _reg[puReg] &= ~(1<<pin);
            } else {
                _reg[puReg] |= (1<<pin);
            }
            writeRegister(puReg);
            break;
    }
}
    
/*! This will return the current state of a pin set to INPUT, or the last
 *  value written to a pin set to OUTPUT.
 *
 *  Example:
 *
 *      byte value = myExpander.digitalRead(4);
 */
uint8_t MCP23S17::digitalRead(uint8_t pin) {
    if (pin >= 16) {
        return 0;
    }
    uint8_t dirReg = MCP_IODIRA;
    uint8_t portReg = MCP_GPIOA;
    uint8_t latReg = MCP_OLATA;
    if (pin >= 8) {
        pin -= 8;
        dirReg = MCP_IODIRB;
        portReg = MCP_GPIOB;
        latReg = MCP_OLATB;
    }

    uint8_t mode = (_reg[dirReg] & (1<<pin)) == 0 ? OUTPUT : INPUT;

    switch (mode) {
        case OUTPUT: 
            return _reg[latReg] & (1<<pin) ? HIGH : LOW;
        case INPUT:
            readRegister(portReg);
            return _reg[portReg] & (1<<pin) ? HIGH : LOW;
    }
    return 0;
}

/*! This function returns the entire 8-bit value of a GPIO port.  Note that
 *  only the bits which correspond to a GPIO pin set to INPUT are valid.
 *  Other pins should be ignored.  The only parameter defines which port (A/B)
 *  to retrieve: 0 is port A and 1 (or anything other than 0) is port B.
 *
 *  Example:
 *
 *      byte portA = myExpander.readPort(0);
 */
uint8_t MCP23S17::readPort(uint8_t port) {
    if (port == 0) {
        readRegister(MCP_GPIOA);
        return _reg[MCP_GPIOA];
    } else {
        readRegister(MCP_GPIOB);
        return _reg[MCP_GPIOB];
    }
}

/*! This is a full 16-bit version of the parameterised readPort function.  This
 *  version reads the value of both ports and combines them into a single 16-bit
 *  value.
 *
 *  Example:
 *
 *      unsigned int value = myExpander.readPort();
 */
uint16_t MCP23S17::readPort() {
    readRegister(MCP_GPIOA);
    readRegister(MCP_GPIOB);
    return (_reg[MCP_GPIOB] << 8) | _reg[MCP_GPIOA];
}

/*! This writes an 8-bit value to one of the two IO port banks (A/B) on the chip.
 *  The value is output direct to any pins on that bank that are set as OUTPUT. Any
 *  bits that correspond to pins set to INPUT are ignored.  As with the readPort
 *  function the first parameter defines which bank to use (0 = A, 1+ = B).
 *
 *  Example:
 *
 *      myExpander.writePort(0, 0x55);
 */
void MCP23S17::writePort(uint8_t port, uint8_t val) {
    if (port == 0) {
        _reg[MCP_OLATA] = val;
        writeRegister(MCP_OLATA);
    } else {
        _reg[MCP_OLATB] = val;
        writeRegister(MCP_OLATB);
    }
}

/*! This is the 16-bit version of the writePort function.  This takes a single
 *  16-bit value and splits it between the two IO ports, the upper half going to
 *  port B and the lower to port A.
 *
 *  Example:
 *
 *      myExpander.writePort(0x55AA);
 */
void MCP23S17::writePort(uint16_t val) {
    _reg[MCP_OLATB] = val >> 8;
    _reg[MCP_OLATA] = val & 0xFF;
    writeRegister(MCP_OLATA);
    writeRegister(MCP_OLATB);
}

/*! This enables the interrupt functionality of a pin.  The interrupt type can be one of:
 *
 *  * CHANGE
 *  * RISING
 *  * FALLING
 *
 *  When an interrupt occurs the corresponding port's INT pin will be driven to it's configured
 *  level, and will remain there until either the port is read with a readPort or digitalRead, or the
 *  captured port status at the time of the interrupt is read using getInterruptValue.
 *
 *  Example:
 * 
 *      myExpander.enableInterrupt(4, RISING);
 */
void MCP23S17::enableInterrupt(uint8_t pin, uint8_t type) {
    if (pin >= 16) {
        return;
    }
    uint8_t intcon = MCP_INTCONA;
    uint8_t defval = MCP_DEFVALA;
    uint8_t gpinten = MCP_GPINTENA;

    if (pin >= 8) {
        pin -= 8;
        intcon = MCP_INTCONB;
        defval = MCP_DEFVALB;
        gpinten = MCP_GPINTENB;
    }

    switch (type) {
        case CHANGE:
            _reg[intcon] &= ~(1<<pin);
            break;
        case RISING:
            _reg[intcon] |= (1<<pin);
            _reg[defval] &= ~(1<<pin);
            break;
        case FALLING:
            _reg[intcon] |= (1<<pin);
            _reg[defval] |= (1<<pin);
            break;

    }

    _reg[gpinten] |= (1<<pin);

    writeRegister(intcon);
    writeRegister(defval);
    writeRegister(gpinten);
}

/*! This disables the interrupt functionality of a pin.
 *
 *  Example:
 *
 *      myExpander.disableInterrupt(4);
 */
void MCP23S17::disableInterrupt(uint8_t pin) {
    if (pin >= 16) {
        return;
    }
    uint8_t gpinten = MCP_GPINTENA;

    if (pin >= 8) {
        pin -= 8;
        gpinten = MCP_GPINTENB;
    }

    _reg[gpinten] &= ~(1<<pin);
    writeRegister(gpinten);
}

/*! The two IO banks can have their INT pins connected together.
 *  This enables you to monitor both banks with just one interrupt pin
 *  on the host microcontroller.  Calling setMirror with a parameter of 
 *  *true* will enable this feature.  Calling it with *false* will disable
 *  it.
 *
 *  Example:
 *
 *      myExpander.setMirror(true);
 */
void MCP23S17::setMirror(bool m) {
    if (m) {
        _reg[MCP_IOCONA] |= (1<<6);
        _reg[MCP_IOCONB] |= (1<<6);
    } else {
        _reg[MCP_IOCONA] &= ~(1<<6);
        _reg[MCP_IOCONB] &= ~(1<<6);
    }
    writeRegister(MCP_IOCONA);
}

/*! This function returns a 16-bit bitmap of the the pin or pins that have cause an interrupt to fire.
 *
 *  Example:
 *
 *      unsigned int pins = myExpander.getInterruptPins();
 */
uint16_t MCP23S17::getInterruptPins() {
    readRegister(MCP_INTFA);
    readRegister(MCP_INTFB);

    return (_reg[MCP_INTFB] << 8) | _reg[MCP_INTFA];
}

/*! This returns a snapshot of the IO pin states at the moment the last interrupt occured.  Reading
 *  this value clears the interrupt status (and hence the INT pins) for the whole chip.
 *  Until this value is read (or the current live port value is read) no further interrupts can
 *  be indicated.
 *
 *  Example:
 *
 *      unsigned int pinValues = myExpander.getInterruptPins();
 */
uint16_t MCP23S17::getInterruptValue() {
    readRegister(MCP_INTCAPA);
    readRegister(MCP_INTCAPB);

    return (_reg[MCP_INTCAPB] << 8) | _reg[MCP_INTCAPA];
} 

/*! This sets the "active" level for an interrupt.  HIGH means the interrupt pin
 *  will go HIGH when an interrupt occurs, LOW means it will go LOW.
 *
 *  Example:
 *
 *      myExpander.setInterruptLevel(HIGH);
 */
void MCP23S17::setInterruptLevel(uint8_t level) {
    if (level == LOW) {
        _reg[MCP_IOCONA] &= ~(1<<1);
        _reg[MCP_IOCONB] &= ~(1<<1);
    } else {
        _reg[MCP_IOCONA] |= (1<<1);
        _reg[MCP_IOCONB] |= (1<<1);
    }
    writeRegister(MCP_IOCONA);
}

/*! Using this function it is possible to configure the interrupt output pins to be open
 *  drain.  This means that interrupt pins from multiple chips can share the same interrupt
 *  pin on the host MCU.  This causes the level set by setInterruptLevel to be ignored.  A
 *  pullup resistor will be required on the host MCU's interrupt pin.
 *
 *  Example:
 *
 *      myExpander.setInterruptOD(true);
 */
void MCP23S17::setInterruptOD(bool openDrain) {
    if (openDrain) {
        _reg[MCP_IOCONA] |= (1<<2);
        _reg[MCP_IOCONB] |= (1<<2);
    } else {
        _reg[MCP_IOCONA] &= ~(1<<2);
        _reg[MCP_IOCONB] &= ~(1<<2);
    }
    writeRegister(MCP_IOCONA);
}
