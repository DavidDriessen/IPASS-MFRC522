#include "hwlib.hpp"
//#include "../hwlib/hwlib-due.hpp"
#include "../../MFRC522/NFCClass.hpp"
#include "ArduinoConsole.cpp"

NFCClass *nfc;
byte data[4][16] = {{0}};

void printBytes(const char *s, byte data[], byte len, bool clean = false) {
    hwlib::cout << s << "\n";
    for (byte i = 0; i < len; i++) {
        hwlib::cout << " 0x";
        if (data[i] < 0x10) {
            hwlib::cout << hwlib::dec << (int) 0;
        }
        hwlib::cout << hwlib::hex << (int) data[i];
    }
    hwlib::cout << "\n";
    if (clean) {
        for (byte i = 0; i < len; i++) {
            data[i] = 0x00;
        }
    }
}

void NFCConsole(ArduinoConsole *pConsole) {
    if (pConsole->checkParameters((char *) "addKey", 1)) {
        int count = 16;
        byte key[16] = {0};
        pConsole->ReadAsByte(key, count);
        nfc->addKey(key);
        return;
    }

    if (pConsole->checkParameters((char *) "getkeys", 1)) {
        byte *key;
        for (int j = 0; j < nfc->getKeyCount(); ++j) {
            key = nfc->getKey(j);
            const char text[3] = {(char) ('1' + j), ':', ' '};
            printBytes(text, key, 6);
        }
        return;
    }
    if (pConsole->checkParameters((char *) "readblock", 1)) {
        NFCClass::Status result = nfc->ReadBlock(pConsole->getParameterAsInt(2), data[0]);
        if (result == nfc->OK) {
            printBytes("Data: ", data[0], 16, true);
        }
        if (result == nfc->AUTH) {
            hwlib::cout << " auth\n";
        }
        if (result == nfc->READ) {
            hwlib::cout << " read\n";
        }
        if (result == nfc->DEAUTH) {
            hwlib::cout << " deauth\n";
        }
        return;
    }
    if (pConsole->checkParameters((char *) "readsector", 1)) {
        NFCClass::Status result = nfc->ReadSector(pConsole->getParameterAsInt(2), (byte **) data);
        if (result == nfc->OK) {
            for (int k = 0; k < 4; ++k) {
                const char text[3] = {(char) ('1' + k), ':', ' '};
                printBytes(text, data[0], 16, true);
            }
            hwlib::cout << "\n";
        }
        if (result == nfc->AUTH) {
            hwlib::cout << " auth\n";
        }
        if (result == nfc->READ) {
            hwlib::cout << " read\n";
        }
        if (result == nfc->DEAUTH) {
            hwlib::cout << " deauth\n";
        }
        return;
    }

    if (pConsole->checkParameters((char *) "writeblock", 1)) {
        int count = 16;
        hwlib::cout << "Data to Write:\n";
        pConsole->ReadAsByte(data[0], count);
        NFCClass::Status result = nfc->WriteBlock(pConsole->getParameterAsInt(2), data[0]);
        if (result == nfc->OK) {
            printBytes("Write: ", data[0], 16);
        }
        if (result == nfc->AUTH) {
            hwlib::cout << " auth\n";
        }
        if (result == nfc->WRITE) {
            hwlib::cout << " write\n";
        }
        if (result == nfc->DEAUTH) {
            hwlib::cout << " deauth\n";
        }
        return;
    }

    hwlib::cout << "Usage: nfc <command> <options>\n";
    hwlib::cout << "\n";
    hwlib::cout << "Commands:\n";
    hwlib::cout << "addKey\n";
    hwlib::cout << "getkeys\n";
    hwlib::cout << "readblock <block>\n";
    hwlib::cout << "readsector <sector>\n";
    hwlib::cout << "\n";
}

int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    hwlib::wait_ms(500);
    hwlib::cout << "START\n";

    SPIsetupClass Default;
    MFRC522 mfrc522(Default);
    NFCClass nfcInit(mfrc522);
    nfc = &nfcInit;

    ArduinoConsole console;
    console.addCommand("nfc", NFCConsole);

    nfc->begin();
    nfc->printFirmwareVersion();

    while (1) {
        console.run();
    }
}
