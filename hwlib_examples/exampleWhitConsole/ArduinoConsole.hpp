//
// Created by david on 6/7/2016.
//

#ifndef TEST_ARDUINOCONSOLE_H
#define TEST_ARDUINOCONSOLE_H

class ArduinoConsole {
    int commandCount = 0;
    int parametersCount = 0;
    char commands[10][20] = {{0}};
    char parameters[5][20] = {{0}};
    int serialInCount = 0;
    char serialInData[120] = {0};

    void (*functions[10])(ArduinoConsole *) = {0};

    void charToByte(char *data, int &count, byte *out);

    int charToInt(char *data);

public:
    ArduinoConsole();

    bool enter = true;

    char *getParameter(int pos = 0);

    bool Read(char *data, int &count);

    void ReadAsByte(byte *data, int &count);

    int ReadAsInt();

    bool getParameterAsByte(byte *data, int pos = 0);

    int getParameterAsInt(int pos = 0);

    bool checkParameters(char *param, int pos = 0);

    void addCommand(const char *com, void (*f)(ArduinoConsole *));

    void run();

    void printCommandList(ArduinoConsole *console);

};


#endif //TEST_ARDUINOCONSOLE_H
