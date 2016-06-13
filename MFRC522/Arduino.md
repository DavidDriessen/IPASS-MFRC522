Arduino implementation          {#Arduino}
============

setup							{#ArduinoSetup}
============
Too use the library it is simple as including the [NFCClass.hpp](@ref NFCClass) file and making an object.

Too begin communicating with the chip, you need to call the “begin(int chipselect)” function.

## example ##					{#ArduinoExample}

~~~~~~~~~~~~~~~{.cpp}
NFCClass nfc;
byte data[16] = {0};
void setup(){
	nfc.begin();
}
void loop(){
	if(nfc.ReadBlock(4,data)){
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
}
~~~~~~~~~~~~~~~
