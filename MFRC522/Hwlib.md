Hwlib implementation          {#Hwlib}
============

setup							{#HwlibSetup}
============
Too use the library you need to including the [MFRC522.hpp](@ref MFRC522) file or for eaze include [NFCClass.hpp](@ref NFCClass).


Too begin communicating with the chip, you need to call the “begin(int chipselect)” function.

## example ##					{#HwlibExample}

~~~~~~~~~~~~~~~{.cpp}
NFCClass *nfc;
byte data[16] = {0};
int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    SPIsetupClass Default;
    MFRC522 mfrc522(Default);
    NFCClass nfcInit(mfrc522);
    nfc = &nfcInit;

    nfc->begin();
    nfc->printFirmwareVersion();

    while (1) {
		if(nfc.ReadBlock(4,data)){
			for (int i = 0; i < 16; ++i) {
				hwlib::cout << " 0x";
				if (data[0][i] < 0x10) {
					hwlib::cout << "0";
				}
			hwlib::cout << hwlib::hex << (int)data[0][i];
			}
			hwlib::cout << "\n";
		}
    }
}

~~~~~~~~~~~~~~~