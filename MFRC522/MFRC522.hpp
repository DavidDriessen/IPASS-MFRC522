//
// Created by david on 5/18/2016.
//
//          Copyright David Driessen 2016 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef IPASS_NFC_H
#define IPASS_NFC_H

#ifndef HWLIB_H
#ifndef _SPI_H_INCLUDED
#error "you have neither hwlib nor Arduino_SPI set up"
#endif
#endif

#include "nfcController.hpp"
#include "SPIsetup.hpp"

#ifdef HWLIB_H
void delay(int ms)
{
    hwlib::wait_ms(ms);
}
#endif

#ifdef _SPI_H_INCLUDED
#ifndef SPIsetup_INIT
#define SPIsetup_INIT
SPIsetupClass SPIsetup;
#endif
#endif

class MFRC522 : public nfcController {
    // List off register addresses
    const static byte CommandReg = 0x01;
    const static byte ComIEnReg = 0x02;
    const static byte DivIEnReg = 0x03;
    const static byte ComIrqReg = 0x04;
    const static byte DivIrqReg = 0x05;
    const static byte ErrorReg = 0x06;
    const static byte Status1Reg = 0x07;
    const static byte Status2Reg = 0x08;
    const static byte FIFODataReg = 0x09;
    const static byte FIFOLevelReg = 0x0A;
    const static byte WaterLevelReg = 0x0B;
    const static byte ControlReg = 0x0C;
    const static byte BitFramingReg = 0x0D;
    const static byte CollReg = 0x0E;
    const static byte ModeReg = 0x11;
    const static byte TxModeReg = 0x12;
    const static byte RxModeReg = 0x13;
    const static byte TxControlReg = 0x14;
    const static byte TxASKReg = 0x15;
    const static byte TxSelReg = 0x16;
    const static byte RxSelReg = 0x17;
    const static byte RxThresholdReg = 0x18;
    const static byte DemodReg = 0x19;
    const static byte MfTxReg = 0x1C;
    const static byte MfRxReg = 0x1D;
    const static byte SerialSpeedReg = 0x1F;
    const static byte CRCResultRegH = 0x21;
    const static byte CRCResultRegL = 0x22;
    const static byte ModWidthReg = 0x24;
    const static byte RFCfgReg = 0x26;
    const static byte GsNReg = 0x27;
    const static byte CWGsPReg = 0x28;
    const static byte ModGsPReg = 0x29;
    const static byte TModeReg = 0x2A;
    const static byte TPrescalerReg = 0x2B;
    const static byte TReloadRegH = 0x2C;
    const static byte TReloadRegL = 0x2D;
    const static byte TCounterValueRegH = 0x2E;
    const static byte TCounterValueRegL = 0x2F;
    const static byte TestSel1Reg = 0x31;
    const static byte TestSel2Reg = 0x32;
    const static byte TestPinEnReg = 0x33;
    const static byte TestPinValueReg = 0x34;
    const static byte TestBusReg = 0x35;
    const static byte AutoTestReg = 0x36;
    const static byte VersionReg = 0x37;
    const static byte AnalogTestReg = 0x38;
    const static byte TestDAC1Reg = 0x39;
    const static byte TestDAC2Reg = 0x3A;
    const static byte TestADCReg = 0x3B;

    // chip commands
    const static byte CMD_Idle = 0x00;
    const static byte CMD_Mem = 0x01;
    const static byte CMD_GenerateRandomID = 0x02;
    const static byte CMD_CalcCRC = 0x03;
    const static byte CMD_Transmit = 0x04;
    const static byte CMD_NoCmdChange = 0x07;
    const static byte CMD_Receive = 0x08;
    const static byte CMD_Transceive = 0x0C;
    const static byte CMD_MFAuthent = 0x0E;
    const static byte CMD_SoftReset = 0x0F;

    // MIFARE card commands
    const static byte APDU_REQA = 0x26;
    const static byte APDU_WUPA = 0x52;
    const static byte APDU_CT = 0x88;
    const static byte APDU_SEL_CL1 = 0x93;
    const static byte APDU_SEL_CL2 = 0x95;
    const static byte APDU_SEL_CL3 = 0x97;
    const static byte APDU_HLTA = 0x50;
    const static byte APDU_MF_AUTH_KEY_A = 0x60;
    const static byte APDU_MF_AUTH_KEY_B = 0x61;
    const static byte APDU_MF_READ = 0x30;
    const static byte APDU_MF_WRITE = 0xA0;
    const static byte APDU_MF_DECREMENT = 0xC0;
    const static byte APDU_MF_INCREMENT = 0xC1;
    const static byte APDU_MF_RESTORE = 0xC2;
    const static byte APDU_MF_TRANSFER = 0xB0;
    const static byte APDU_UL_WRITE = 0xA2;

    int chipSelect = 0;

    byte ReadReg(byte reg) {
        byte read[2] = {0};
        byte write[2] = {0};
        write[0] = (byte)(0x80 | ((reg << 1) & 0x7E));
        write[1] = 0x00;
        SPIsetup.write_and_read(chipSelect, 2, write, read);
        return read[1];
    }

    void ReadReg(byte reg, byte count, byte *values) {
        count++;
        byte out[30] = {0};
        byte in[30] = {0};
        for (int i = 0; i < count - 1; i++) {
            out[i] = (byte)(0x80 | ((reg << 1) & 0x7E));
        }
        out[count] = 0x00;
        SPIsetup.write_and_read(chipSelect, count, out, in);
        for (int i = 0; i < count; i++) {
            values[i] = in[i + 1];
        }
    }

    void WriteReg(byte reg, byte value) {
        byte read[2] = {0};
        byte write[2] = {0};
        write[0] = (byte)((reg << 1) & 0x7E);
        write[1] = value;
        SPIsetup.write_and_read(chipSelect, 2, write, read);
    }

    void WriteReg(byte reg, byte count, byte *values) {
        count++;
        byte out[30] = {0};
        for (int i = 1; i < count; i++) {
            out[i] = values[i - 1];
        }
        out[0] = (byte)((reg << 1) & 0x7E);
        SPIsetup.write_and_read(chipSelect, count, out, nullptr);
    }

    void ClearBitMask(byte reg, byte mask) {
        byte tmp = ReadReg(reg);
        WriteReg(reg, tmp & (~mask));
    }

    void SetBitMask(byte reg, byte mask) {
        byte tmp = ReadReg(reg);
        WriteReg(reg, tmp | mask);
    }

    void Reset() {
        WriteReg(CommandReg, CMD_SoftReset);
        delay(50);
        while (ReadReg(CommandReg) & 0x08) {
        }
    }

    bool Communicate(byte command,
                     byte *sendData,
                     byte sendLen,
                     byte *backData = nullptr,
                     byte *backLen = nullptr,
                     byte *validBits = nullptr,
                     bool checkCRC = false) {
        byte waitIRq = 0, n, _validBits = 0;

        byte bitFraming = (byte)(validBits ? *validBits : 0);

        if (command == CMD_Transceive) {
            waitIRq = 0x30;
        } else if (command == CMD_MFAuthent) {
            waitIRq = 0x10;
        }
        WriteReg(CommandReg, CMD_Idle);

        WriteReg(ComIrqReg, 0x7F);
        SetBitMask(FIFOLevelReg, 0x80);
        WriteReg(FIFODataReg, sendLen, sendData);
        WriteReg(BitFramingReg, bitFraming);
        WriteReg(CommandReg, command);
        if (command == CMD_Transceive) {
            SetBitMask(BitFramingReg, 0x80);
        }

        for (int i = 2000; i > 0;) {
            n = ReadReg(ComIrqReg);
            if (n & waitIRq) {
                break;
            }
            if (n & 0x01) {
                return 0;
            }
            if (--i == 0) {
                return 0;
            }
        }

        byte errorRegValue = ReadReg(ErrorReg);
        if (errorRegValue & 0x13) {
            return 0;
        }

        if (backData && backLen) {
            n = ReadReg(FIFOLevelReg);
            if (n > *backLen || n == 0) {
                return 0;
            }
            *backLen = n;
            ReadReg(FIFODataReg, n, backData);
            _validBits = (byte)(ReadReg(ControlReg) & 0x07);
            if (validBits) {
                *validBits = _validBits;
            }
        }

        if (errorRegValue & 0x08) {
            return 0;
        }

        if (backData && backLen && checkCRC) {
            byte controlBuffer[2];
            if (!calculate_crc(&backData[0], (byte)(*backLen - 2), &controlBuffer[0])) {
                return 0;
            }
            if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1])) {
                return 0;
            }
        }
        return 1;
    }

    bool calculate_crc(byte *data, int length, byte *result) {
        WriteReg(CommandReg, CMD_Idle);
        WriteReg(DivIrqReg, 0x04);
        SetBitMask(FIFOLevelReg, 0x80);
        WriteReg(FIFODataReg, length, data);
        WriteReg(CommandReg, CMD_CalcCRC);

        int i = 5000;
        byte n;
        while (1) {
            n = ReadReg(DivIrqReg);
            if (n & 0x04) {
                break;
            }
            if (--i == 0) {
                return 0;
            }
        }
        WriteReg(CommandReg, CMD_Idle);
        result[0] = ReadReg(CRCResultRegL);
        result[1] = ReadReg(CRCResultRegH);
        return 1;
    }

#ifdef HWLIB_H
    SPIsetupClass& SPIsetup;

public:
    MFRC522(SPIsetupClass& SPI)
        : SPIsetup(SPI)
    {
    }
#else
public:
#endif

    void begin(int chipSelect = 0) {
#ifdef _SPI_H_INCLUDED
        SPIsetup.begin();
#endif
        this->chipSelect = chipSelect;

        if (SPIsetup.readReset() == 0) {
            SPIsetup.writeReset(1);
            delay(50);
        } else {
            Reset();
        }

        WriteReg(TModeReg, 0x80);
        WriteReg(TPrescalerReg, 0x9A);
        WriteReg(TReloadRegH, 0x03);
        WriteReg(TReloadRegL, 0xE8);

        WriteReg(TxASKReg, 0x40);
        WriteReg(ModeReg, 0x3D);

        byte value = ReadReg(TxControlReg);
        if ((value & 0x03) != 0x03) {
            SetBitMask(TxControlReg, 0x03);
        }
    }

    bool DetectCard() {
        byte buffer[2];
        byte len = sizeof(buffer);
        byte validBits = 7;

        ClearBitMask(CollReg, 0x80);
        byte command[1] = {0x26};
        if (!Communicate(CMD_Transceive, command, 1, buffer, &len, &validBits)) {
            return 0;
        }
        if (len != 2 || validBits != 0) {
            return 0;
        }
        return 1;
    }

    bool GetCardId(byte *uid) {
        byte buffer[9];
        byte backLen = 9;

        ClearBitMask(CollReg, 0x80);
        WriteReg(BitFramingReg, 0);
        buffer[0] = 0x93;
        buffer[1] = 0x20;

        if (!Communicate(CMD_Transceive, buffer, (byte) 2, buffer, &backLen, 0)) {
            return 0;
        }
        for (int i = 0; i < 4; i++) {
            uid[i] = buffer[i];
        }

        return 1;
    }

    bool SelectCard(byte *uid) {
        byte index = 2;
        byte buffer[9];
        byte *responseBuffer;
        byte responseLength;

        ClearBitMask(CollReg, 0x80);

        buffer[0] = APDU_SEL_CL1;
        buffer[1] = 0x70;

        for (int count = 0; count < 4; count++) {
            buffer[index++] = uid[count];
        }

        buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
        if (!calculate_crc(buffer, 7, &buffer[7])) {
            return 0;
        }

        responseBuffer = &buffer[6];
        responseLength = 3;

        if (!Communicate(CMD_Transceive, buffer, 9, responseBuffer, &responseLength)) {
            return 0;
        }

        if (responseLength != 3) {
            return 0;
        }
        if (!calculate_crc(responseBuffer, 1, &buffer[2])) {
            return 0;
        }
        if ((buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2])) {
            return 0;
        }
        return 1;
    }

    bool Authenticate(byte authType, byte blockAddr, byte *key, byte *uid) {
        byte sendData[12];
        sendData[0] = authType;
        sendData[1] = blockAddr;
        for (byte i = 0; i < 6; i++) {
            sendData[2 + i] = key[i];
        }
        for (byte i = 0; i < 4; i++) {
            sendData[8 + i] = uid[i];
        }
        return Communicate(CMD_MFAuthent, &sendData[0], sizeof(sendData));
    }

    bool Deauthenticate() {
        byte buffer[4] = {APDU_HLTA, 0x00};
        if (!calculate_crc(buffer, 2, &buffer[2])) {
            return 0;
        }
        byte n;
        WriteReg(CommandReg, CMD_Idle);
        WriteReg(ComIrqReg, 0x7F);
        SetBitMask(FIFOLevelReg, 0x80);
        WriteReg(FIFODataReg, 4, buffer);
        WriteReg(BitFramingReg, 0x00);
        WriteReg(CommandReg, CMD_Transceive);
        SetBitMask(BitFramingReg, 0x80);

        for (int i = 2000; i > 0;) {
            n = ReadReg(ComIrqReg);
            if (n & 0x30) {
                break;
            }
            if (n & 0x01) {
                ClearBitMask(Status2Reg, 0x08);
                return 1;
            }
            if (--i == 0) {
                ClearBitMask(Status2Reg, 0x08);
                return 1;
            }
        }
        return 0;
    }

    bool ReadBlock(byte block_address, byte *data, byte &dataLen) {
        if (data == nullptr || dataLen > 16) {
            return 0;
        }

        byte buf[4] = {0};
        byte responseData[18] = {0};
        byte responseLen = 18;
        buf[0] = APDU_MF_READ;
        buf[1] = block_address;
        calculate_crc(buf, 2, &buf[2]);
        if (Communicate(CMD_Transceive, buf, 4, responseData, &responseLen)) {
            for (int i = 0; i < dataLen; ++i) {
                data[i] = responseData[i];
            }
            return 1;
        }
        return 0;
    }

    bool WriteBlock(byte blockAddr, byte *dataToWrite, byte dataLen) {
        if (dataToWrite == nullptr || dataLen != 16) {
            return 0;
        }
        byte bufLen = 4;
        byte validBits = 0;
        byte buf[4] = {APDU_MF_WRITE, blockAddr};
        if (!calculate_crc(buf, 2, &buf[2])) {
            return 0;
        }
        if (!Communicate(CMD_Transceive, buf, 4, buf, &bufLen, &validBits)) {
            return 0;
        }
        if (bufLen != 1 || validBits != 4) {
            return 0;
        }
        if (buf[0] != 0xA) {
            return 0;
        }

        byte writebuf[18] = {0};

        for (int i = 0; i < dataLen; ++i) {
            writebuf[i] = dataToWrite[i];
        }
        if (!calculate_crc(writebuf, dataLen, &writebuf[dataLen])) {
            return 0;
        }
        dataLen += 2;

        byte writebufLen = 18;
        validBits = 0;
        if (!Communicate(CMD_Transceive, writebuf, dataLen, writebuf, &writebufLen, &validBits)) {
            return 0;
        }

        if (writebufLen != 1 || validBits != 4) {
            return 0;
        }
        if (writebuf[0] != 0xA) {
            return 0;
        }
        return 1;
    }

    byte getFirmwareVersion() {

        byte v = ReadReg(VersionReg);
        return v;
    }
};

#endif
