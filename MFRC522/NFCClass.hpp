//
// Created by david on 6/8/2016.
//
//          Copyright David Driessen 2016 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_NFCCLASS_H
#define TEST_NFCCLASS_H

#ifndef HWLIB_H
#ifndef _SPI_H_INCLUDED
#  error "you have neither hwlib nor Arduino_SPI set up"
#endif
#endif

#include "MFRC522.hpp"

#ifdef _SPI_H_INCLUDED
MFRC522 mfrc522;
#endif

/**
 * The NFCClass class is an simplified version of the implementation of a nfc reader.
 *
 * /author David Driessen
 */
class NFCClass {
    void SerialPrint(const char *data, int hex = 10) {
        if (hex == 16) {
            //Serial.print(data, HEX);
            hwlib::cout << hwlib::hex << data;
        } else {
            //Serial.print(data);
            hwlib::cout << data;
        }
    }

    void SerialPrint(const byte &data, int hex = 10) {
        if (hex == 16) {
            //Serial.print(data, HEX);
            hwlib::cout << hwlib::hex << (int) data;
        } else {
            //Serial.print(data);
            hwlib::cout << (int) data;
        }
    }

public:
    enum Status : byte {
        OK,
        ERROR,
        AUTH,
        DEAUTH,
        READ,
        WRITE
    };

#ifdef _SPI_H_INCLUDED
    /**
   * Default constructor
   */
    NFCClass() : nfc(mfrc522) { }

#endif

    /**
     * Constructor
     * \param nfcc An implementation of the nfcController class
     */
    NFCClass(nfcController &nfcc) : nfc(nfcc) { }

    /**
     * Set things up.
     * \param chipSelect The number of the chipSelect pin where the chip is connected.
     */
    void begin(int chipSelect = 0) {
        nfc.begin(chipSelect);
    }

    /**
     * Read an block of an RFID card.
     * \param block The number of the block that you want too read.
     * \param data An array with min size 16, to put the data in.
     * \param loop Wait for the data. Default = true.
     */
    Status ReadBlock(int block, byte *data, bool loop = true) {
        for (int j = 0; j < keysLen; ++j) {
            if (!prepare(loop)) {
                return ERROR;
            }

            if (nfc.Authenticate(0x60, block, keys[j], uid)) {
                Status result = Read(block, data);
                if (result != OK) {
                    return result;
                }

                if (!nfc.Deauthenticate()) {
                    return DEAUTH;
                }
                return OK;
            }
        }
        return AUTH;
    }

    /**
     * Read an sector of an RFID card.
     * \param sector The number of the sector that you want too read.
     * \param data An array (with min size 4) of arrays (with min size 16), to put the data in.
     * \param loop Wait for the data. Default = true.
     */
    Status ReadSector(int sector, byte **data, bool loop = true) {
        sector *= 4;
        for (int j = 0; j < keysLen; ++j) {
            if (!prepare(loop)) {
                return ERROR;
            }
            if (nfc.Authenticate(0x60, sector, keys[j], uid)) {
                for (int block = sector; block < sector + 4; ++block) {
                    Status result = Read(block, data[block - sector]);
                    if (result != OK) {
                        return result;
                    }
                }
                if (!nfc.Deauthenticate()) {
                    return DEAUTH;
                }
                return OK;
            }
        }
        return AUTH;
    }

    /**
     * Read all data of an RFID card.
     * \param sector The number of the sector that you want too read.
     * \param data An array (with min size 16) of arrays (with min size 4) of arrays (with min size 16), to put the data in.
     * \param loop Wait for the data. Default = true.
     */
    Status ReadAll(byte ***data, bool loop = true, int key = 0) {
        for (int sector = 0; sector < 16; ++sector) {
            Status result = ReadSector(sector, data[sector], loop);
            if (result != OK) {
                return result;
            }
        }
    }

    /**
     * Write data to an block of an RFID card.
     * \param block The number of the block that you want too read.
     * \param data An array with data to write to the block. Max site 16.
     * \param loop Wait for the data. Default = true.
     */
    Status WriteBlock(int block, byte *data, bool loop = true) {
        for (int j = 0; j < keysLen; ++j) {
            if (!prepare(loop)) {
                return ERROR;
            }

            if (nfc.Authenticate(0x60, block, keys[j], uid)) {
                Status result = Write(block, data);
                if (result != OK) {
                    return result;
                }

                if (!nfc.Deauthenticate()) {
                    return DEAUTH;
                }
                return OK;
            }
        }
        return AUTH;
    }

    /**
     * Get the version of the chips firmware.
     * \return The firmware version as byte.
     */
    byte getFirmwareVersion() {
        return nfc.getFirmwareVersion();
    }

    /**
     * Print the version of the chips firmware.
     */
    void printFirmwareVersion() {
        byte v = nfc.getFirmwareVersion();
        SerialPrint("Firmware Version: 0x");
        SerialPrint(v, HEX);

        switch (v) {
            case 0x88:
                SerialPrint(" = (clone)");
                break;
            case 0x90:
                SerialPrint(" = v0.0");
                break;
            case 0x91:
                SerialPrint(" = v1.0");
                break;
            case 0x92:
                SerialPrint(" = v2.0");
                break;
            default:
                SerialPrint(" = (unknown)");
        }
        SerialPrint("\n");

        if ((v == 0x00) || (v == 0xFF)) {
            SerialPrint("WARNING: Communication failure, is the MFRC522 properly connected?\n");
        }
    }

    /**
     * Adds an 6 bytes long key to the key list.
     * \param key The 6 bytes long array key to add to the key list.
     */
    void addKey(byte *key) {
        for (int i = 0; i < 16; ++i) {
            keys[keysLen][i] = key[i];
        }
        for (int j = 16; j < 16; ++j) {
            keys[keysLen][j] = 0x00;
        }
        keysLen++;
    }

    /**
     * Get the key from the key list.
     * \param pos The position of the key in de list.
     * \return The 6 bytes long array key from the key list.
     */
    byte *getKey(int pos) {
        if (pos >= keysLen) {
            return nullptr;
        }
        return keys[pos];
    }

    /**
     * Get the number of keys in the key list.
     * \return Number of keys in the key list.
     */
    int getKeyCount() {
        return keysLen;
    }

    /**
     * Get the id of the last scanned card.
     * \return An 4 bytes id.
     */
    byte *getId() {
        return uid;
    }

private:
    nfcController &nfc;
    int keysLen = 3;
    byte keys[10][6] = {
            {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
            {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7},
            {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}
    };
    byte uid[4] = {0};

    bool prepare(bool loop = true) {
        if (loop) {
            while (!nfc.DetectCard());
        } else {
            if (!nfc.DetectCard()) {
                return false;
            }
        }
        //Serial.println("Card detected");
        if (!nfc.GetCardId(uid)) {
            return false;
        }
        /*Serial.print("Uid: ");
        for (byte i = 0; i < 4; i++) {
            Serial.print(uid[i], HEX);
            Serial.print(" ");
        }
        Serial.println();*/

        if (!nfc.SelectCard(uid)) {
            return false;
        }
        return true;
    }

    Status Read(int block, byte *data) {
        byte len = 16;
        if (nfc.ReadBlock(block, data, len)) {
            return OK;
        } else {
            return READ;
        }
    }

    Status Write(int block, byte *data) {
        byte len = 16;
        if (nfc.WriteBlock(block, data, len)) {
            return OK;
        } else {
            return WRITE;
        }
    }

};


#endif //TEST_NFCCLASS_H
