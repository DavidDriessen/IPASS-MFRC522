#include <Servo.h>
#include "SPI.h"
#include "../../MFRC522/NFCClass.hpp"
#include "ArduinoConsole.hpp"

NFCClass nfc;
ArduinoConsole console;

Servo lock;
bool access;
bool caccess = false;
NFCClass::Status result;
byte control[2][16] = {{0},
                       {1}};
byte data[4][16] = {{0}};

void NFCConsole(ArduinoConsole *pConsole) {
    if (pConsole->checkParameters((char *) "addKey", 1)) {
        int count = 16;
        byte key[16] = {0};
        pConsole->ReadAsByte(key, count);
        nfc.addKey(key);
        return;
    }

    if (pConsole->checkParameters((char *) "getkeys", 1)) {
        byte *key;
        for (int j = 0; j < nfc.getKeyCount(); ++j) {
            key = nfc.getKey(j);
            for (int i = 0; i < 6; ++i) {
                Serial.print(key[i], HEX);
                Serial.print(" ");
            }
            Serial.println();
        }
        return;
    }
    if (pConsole->checkParameters((char *) "readblock", 1)) {
        if (nfc.ReadBlock(pConsole->getParameterAsInt(2), data[0]) == nfc.OK) {
            for (int i = 0; i < 16; ++i) {
                Serial.print(" ");
                Serial.print("0x");
                if (data[0][i] < 0x10) {
                    Serial.print("0");
                }
                Serial.print(data[0][i], HEX);
            }
            Serial.println();
        }
        if (result == nfc.AUTH) {
            Serial.println(" auth");
        }
        if (result == nfc.READ) {
            Serial.println(" read");
        }
        if (result == nfc.DEAUTH) {
            Serial.println(" deauth");
        }
        return;
    }

    if (pConsole->checkParameters((char *) "readblockasstring", 1)) {
        if (nfc.ReadBlock(pConsole->getParameterAsInt(2), data[0]) == nfc.OK) {
            Serial.println((char *) data[0]);
        }
        if (result == nfc.AUTH) {
            Serial.println(" auth");
        }
        if (result == nfc.READ) {
            Serial.println(" read");
        }
        if (result == nfc.DEAUTH) {
            Serial.println(" deauth");
        }
        return;
    }

    if (pConsole->checkParameters((char *) "readsector", 1)) {
        NFCClass::Status result = nfc.ReadSector(pConsole->getParameterAsInt(2), (byte **) data);
        if (result == nfc.OK) {
            for (int k = 0; k < 4; ++k) {
                for (int i = 0; i < 16; ++i) {
                    Serial.print(" ");
                    Serial.print("0x");
                    if (data[k][i] < 0x10) {
                        Serial.print("0");
                    }
                    Serial.print(data[k][i], HEX);
                }
                Serial.println();
            }
            Serial.println();
        }
        if (result == nfc.AUTH) {
            Serial.println(" auth");
        }
        if (result == nfc.READ) {
            Serial.println(" read");
        }
        if (result == nfc.DEAUTH) {
            Serial.println(" deauth");
        }
        return;
    }

    if (pConsole->checkParameters((char *) "writeblock", 1)) {
        int count = 16;
        Serial.println("Data to Write:");
        pConsole->ReadAsByte(data[0], count);
        if (nfc.WriteBlock(pConsole->getParameterAsInt(2), data[0]) == nfc.OK) {
            for (int i = 0; i < 16; ++i) {
                Serial.print(" ");
                Serial.print("0x");
                if (data[0][i] < 0x10) {
                    Serial.print("0");
                }
                Serial.print(data[0][i], HEX);
            }
            Serial.println();
        }
        if (result == nfc.AUTH) {
            Serial.println(" auth");
        }
        if (result == nfc.WRITE) {
            Serial.println(" write");
        }
        if (result == nfc.DEAUTH) {
            Serial.println(" deauth");
        }
        return;
    }

    if (pConsole->checkParameters((char *) "writeblockasstring", 1)) {
        int count = 16;
        Serial.println("Data to Write:");
        pConsole->Read((char *) data[0], count);
        if (nfc.WriteBlock(pConsole->getParameterAsInt(2), data[0]) == nfc.OK) {
            Serial.println((char *) data[0]);
        }
        if (result == nfc.AUTH) {
            Serial.println(" auth");
        }
        if (result == nfc.WRITE) {
            Serial.println(" write");
        }
        if (result == nfc.DEAUTH) {
            Serial.println(" deauth");
        }
        return;
    }

    Serial.println("Usage: nfc <command> <options>");
    Serial.println("");
    Serial.println("Commands:");
    Serial.println("addKey");
    Serial.println("getkeys");
    Serial.println("readblock <block>");
    Serial.println("readsector <sector>");
    Serial.println("");
}

void setup() {
    nfc.begin();
    console.begin();
    Serial.println("Start");
    nfc.printFirmwareVersion();
    console.addCommand("nfc", NFCConsole);
    lock.attach(11);
    lock.write(20);
    Serial.println("Present card");
}

void loop() {
    result = nfc.ReadBlock(4, data[0], false);
    if (result == nfc.OK) {
        access = true;
        for (int i = 0; i < 16; ++i) {
            if (data[0][i] != control[0][i]) {
                access = false;
                break;
            }
        }

        if (access) {
            Serial.println("access");
            lock.write(170);
            delay(1000);
            lock.write(10);
        } else {
            caccess = true;
            for (int i = 0; i < 16; ++i) {
                if (data[0][i] != control[1][i]) {
                    caccess = false;
                    break;
                }
            }
            if (!caccess) {
                Serial.println("No access");
            } else {
                while (Serial.read() != -1) { }
                console.enter = false;
                Serial.print("Arduino:\t");
            }
        }

    } else if (result == nfc.AUTH) {
        caccess = false;
        Serial.println("No access");
    }
    if (caccess) {
        console.run(false);
    }
}