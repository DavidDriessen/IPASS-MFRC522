//
// Created by david on 5/18/2016.
//

#ifndef IPASS_NFCABSTRACT_H
#define IPASS_NFCABSTRACT_H

class nfcController {
public:

    virtual void begin(int chipSelect = 0) = 0;

    virtual bool DetectCard() = 0;

    virtual bool GetCardId(byte *uid) = 0;

    virtual bool SelectCard(byte *uid) = 0;

    virtual bool Authenticate(byte authType, byte blockAddr, byte *key, byte *uid) = 0;

    virtual bool Deauthenticate() = 0;

    virtual bool ReadBlock(byte block_address, byte *data, byte &dataLe) = 0;

    virtual bool WriteBlock(byte blockAddr, byte *dataToWrite, byte dataLen) = 0;

    virtual byte getFirmwareVersion() = 0;
};

#endif //IPASS_NFCABSTRACT_H
