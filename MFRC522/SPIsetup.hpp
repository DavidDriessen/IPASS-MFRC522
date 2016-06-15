//
// Created by david on 5/18/2016.
//
//          Copyright David Driessen 2016 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef IPASS_SPISETUP_H
#define IPASS_SPISETUP_H

#ifdef HWLIB_H
/**
* Controller for the SPI protocol in the hwlib library.
*
* /author David Driessen
*/
class SPIsetupClass {
    hwlib::target::pin_out chipSelect0;
    hwlib::target::pin_out chipSelect1;
    hwlib::target::pin_in_out resetPin;
    hwlib::target::pin_out sck;
    hwlib::target::pin_out mosi;
    hwlib::target::pin_in miso;
    hwlib::spi_bus_bit_banged_sclk_mosi_miso SPI;
public:

    /**
     * Setup the default SPI protocol.
     *
     * Default:
     * chipSelect0 = pin 10.
     * chipSelect1 = pin 50.
     */
    SPIsetupClass() :
            chipSelect0(hwlib::target::pins::cs0),
            chipSelect1(hwlib::target::pins::cs1),
            resetPin(hwlib::target::pins::d9),
            sck(hwlib::target::pins::sck),
            mosi(hwlib::target::pins::mosi),
            miso(hwlib::target::pins::miso),
            SPI(sck, mosi, miso) {
        chipSelect0.set(1);
        chipSelect1.set(1);
        hwlib::wait_ms(5);
    }

    /**
     * Setup the SPI protocol with use defined pins.
     *
     * /param sckPin hwlib pin.
     */
    SPIsetupClass(hwlib::target::pins sckPin, hwlib::target::pins mosiPin, hwlib::target::pins misoPin, hwlib::target::pins cs0 = hwlib::target::pins::cs0, hwlib::target::pins cs1 = hwlib::target::pins::cs1) :
            chipSelect0(cs0),
            chipSelect1(cs1),
            resetPin(hwlib::target::pins::d9),
            sck(sckPin),
            mosi(mosiPin),
            miso(misoPin),
            SPI(sck, mosi, miso) { }

    void write_and_read(int chipSelect, int count, byte out[], byte in[]) {
        switch (chipSelect) {
            case 0:
                SPI.write_and_read(chipSelect0, count, out, in);
                break;
            case 1:
                SPI.write_and_read(chipSelect1, count, out, in);
                break;
            default:
                SPI.write_and_read(chipSelect0, count, out, in);
                break;
        }
    }

    bool readReset() {
        resetPin.direction_set_input();
        return resetPin.get();
    }

    void writeReset(bool bit) {
        resetPin.direction_set_output();
        resetPin.set(bit);
    }
};
#endif //HWLIB_H

#ifdef _SPI_H_INCLUDED

/**
* Controller to setup the SPI protocol in the Arduino library.
*/
class SPIsetupClass {
    uint32_t chipSelect0 = 10;
    uint32_t chipSelect1 = 50;
    int reset = 9;

public:
    /**
     * Start the setup for the SPI protocol.
     *
     * /param chipSelect0 Pin for the chipselect of the device. Default = 10
     * /param chipSelect1 Pin for the chipselect of the device. Default = 50
     */
    void begin(uint32_t chipSelect0 = 10, uint32_t chipSelect1 = 50) {
        this->chipSelect0 = chipSelect0;
        this->chipSelect1 = chipSelect1;
        SPI.begin();
        pinMode(chipSelect0, OUTPUT);
        pinMode(chipSelect1, OUTPUT);
    }

    void write_and_read(int chipSelect, int count, byte out[], byte in[]) {
        if (count == 0) {
            return;
        }
        uint32_t _chipSelectPin;
        switch (chipSelect) {
            case 0:
                _chipSelectPin = chipSelect0;
                break;
            case 1:
                _chipSelectPin = chipSelect1;
                break;
            default:
                _chipSelectPin = chipSelect0;
                break;
        }

        SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV4, MSBFIRST, SPI_MODE0));
        digitalWrite(_chipSelectPin, LOW);
        for (int i = 0; i < count; ++i) {
            in[i] = SPI.transfer(out[i]);
        }
        digitalWrite(_chipSelectPin, HIGH);
        SPI.endTransaction();
    }

    bool readReset() {
        pinMode(reset, INPUT);
        return (bool) digitalRead(reset);
    }

    void writeReset(bool bit) {
        pinMode(reset, OUTPUT);
        digitalWrite(reset, bit);
    }
};

#endif //_SPI_H_INCLUDED
#endif //IPASS_SPISETUP_H
