#include "hwlib.hpp"
//#include "../hwlib/hwlib-due.hpp"
#include "../../MFRC522/NFCClass.hpp"
#include "ArduinoConsole.cpp"

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

int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    hwlib::wait_ms(500);
    hwlib::cout << "START\n";

    SPIsetupClass Default;
    MFRC522 mfrc522(Default);
    NFCClass nfc(mfrc522);

    ArduinoConsole console;

    nfc.begin();

    hwlib::cout << hwlib::hex << (int) nfc.getFirmwareVersion();

    while (1) {
        console.run();
    }
}
