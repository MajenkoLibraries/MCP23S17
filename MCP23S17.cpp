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

#ifdef __PIC32MX__
MCP23S17::MCP23S17(DSPI *spi, uint8_t cs, uint8_t addr) {
#else
MCP23S17::MCP23S17(SPIClass *spi, uint8_t cs, uint8_t addr) {
#endif
    _spi = spi;
    _cs = cs;
    _addr = addr;

    _reg[IODIRA] = 0xFF;
    _reg[IODIRB] = 0xFF;
    _reg[IPOLA] = 0x00;
    _reg[IPOLB] = 0x00;
    _reg[GPINTENA] = 0x00;
    _reg[GPINTENB] = 0x00;
    _reg[DEFVALA] = 0x00;
    _reg[DEFVALB] = 0x00;
    _reg[INTCONA] = 0x00;
    _reg[INTCONB] = 0x00;
    _reg[IOCONA] = 0x18;
    _reg[IOCONB] = 0x18;
    _reg[GPPUA] = 0x00;
    _reg[GPPUB] = 0x00;
    _reg[INTFA] = 0x00;
    _reg[INTFB] = 0x00;
    _reg[INTCAPA] = 0x00;
    _reg[INTCAPB] = 0x00;
    _reg[GPIOA] = 0x00;
    _reg[GPIOB] = 0x00;
    _reg[OLATA] = 0x00;
    _reg[OLATB] = 0x00;
}

void MCP23S17::begin() {
    _spi->begin();
    ::pinMode(_cs, OUTPUT);
    ::digitalWrite(_cs, HIGH);
    uint8_t cmd = 0b01000000;
    ::digitalWrite(_cs, LOW);
    _spi->transfer(cmd);
    _spi->transfer(IOCONA);
    _spi->transfer(0x18);
    ::digitalWrite(_cs, HIGH);
    writeAll();
}

void MCP23S17::readRegister(uint8_t addr) {
    if (addr > 21) {
        return;
    }
    uint8_t cmd = 0b01000001 | ((_addr & 0b111) << 1);
    ::digitalWrite(_cs, LOW);
    _spi->transfer(cmd);
    _spi->transfer(addr);
    _reg[addr] = _spi->transfer(0xFF);
    ::digitalWrite(_cs, HIGH);
}

void MCP23S17::writeRegister(uint8_t addr) {
    if (addr > 21) {
        return;
    }
    uint8_t cmd = 0b01000000 | ((_addr & 0b111) << 1);
    ::digitalWrite(_cs, LOW);
    _spi->transfer(cmd);
    _spi->transfer(addr);
    _spi->transfer(_reg[addr]);
    ::digitalWrite(_cs, HIGH);
}

void MCP23S17::readAll() {
    uint8_t cmd = 0b01000001 | ((_addr & 0b111) << 1);
    ::digitalWrite(_cs, LOW);
    _spi->transfer(cmd);
    _spi->transfer(0);
    for (uint8_t i = 0; i < 22; i++) {
        _reg[i] = _spi->transfer(0xFF);
    }
    ::digitalWrite(_cs, HIGH);
}

void MCP23S17::writeAll() {
    uint8_t cmd = 0b01000000 | ((_addr & 0b111) << 1);
    ::digitalWrite(_cs, LOW);
    _spi->transfer(cmd);
    _spi->transfer(0);
    for (uint8_t i = 0; i < 22; i++) {
        _spi->transfer(_reg[i]);
    }
    ::digitalWrite(_cs, HIGH);
}
    
void MCP23S17::pinMode(uint8_t pin, uint8_t mode) {
    if (pin >= 16) {
        return;
    }
    uint8_t dirReg = IODIRA;
    uint8_t puReg = GPPUA;
    if (pin >= 8) {
        pin -= 8;
        dirReg = IODIRB;
        puReg = GPPUB;
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

void MCP23S17::digitalWrite(uint8_t pin, uint8_t value) {
    if (pin >= 16) {
        return;
    }
    uint8_t dirReg = IODIRA;
    uint8_t puReg = GPPUA;
    uint8_t latReg = OLATA;
    if (pin >= 8) {
        pin -= 8;
        dirReg = IODIRB;
        puReg = GPPUB;
        latReg = OLATB;
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
    
uint8_t MCP23S17::digitalRead(uint8_t pin) {
    if (pin >= 16) {
        return 0;
    }
    uint8_t dirReg = IODIRA;
    uint8_t portReg = GPIOA;
    uint8_t latReg = OLATA;
    if (pin >= 8) {
        pin -= 8;
        dirReg = IODIRB;
        portReg = GPIOB;
        latReg = OLATB;
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
