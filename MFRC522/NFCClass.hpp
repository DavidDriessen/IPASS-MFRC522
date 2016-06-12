//
// Created by david on 6/8/2016.
//

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

class NFCClass {
    void SerialPrint(const char *data, int hex = 10) {
        if (hex == 16) {
#ifdef _SPI_H_INCLUDED
            Serial.print(data, HEX);
#endif
#ifdef HWLIB_H
            hwlib::cout << hwlib::hex << data;
#endif
        } else {
#ifdef _SPI_H_INCLUDED
            Serial.print(data);
#endif
#ifdef HWLIB_H
            hwlib::cout << data;
#endif
        }
    }

    void SerialPrint(const byte &data, int hex = 10) {
        if (hex == 16) {
#ifdef _SPI_H_INCLUDED
            Serial.print(data, HEX);
#endif
#ifdef HWLIB_H
            hwlib::cout << hwlib::hex << (int)data;
#endif
        } else {
#ifdef _SPI_H_INCLUDED
            Serial.print(data);
#endif
#ifdef HWLIB_H
            hwlib::cout << (int)data;
#endif
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

    NFCClass() : nfc(mfrc522) { }

#endif

    NFCClass(nfcController &nfcc) : nfc(nfcc) { }

    void begin(int cs = 0) {
        nfc.begin();
    }

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

    Status ReadAll(byte ***data, bool loop = true, int key = 0) {
        for (int sector = 0; sector < 16; ++sector) {
            Status result = ReadSector(sector, data[sector], loop);
            if (result != OK) {
                return result;
            }
        }
    }

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

    byte getFirmwareVersion() {
        return nfc.getFirmwareVersion();
    }

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

    void addKey(byte *key, int len) {
        if (len > 16) {
            return;
        }
        for (int i = 0; i < len; ++i) {
            keys[keysLen][i] = key[i];
        }
        for (int j = len; j < 16; ++j) {
            keys[keysLen][j] = 0x00;
        }
        keysLen++;
    }

    byte *getKey(int pos) {
        if (pos >= keysLen) {
            return nullptr;
        }
        return keys[pos];
    }

    int getKeyCount() {
        return keysLen;
    }

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
