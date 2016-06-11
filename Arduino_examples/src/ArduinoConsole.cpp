//
// Created by david on 6/7/2016.
//
#include "ArduinoConsole.hpp"

void printCommands(ArduinoConsole *console) {
    console->printCommandList(console);
}

void ArduinoConsole::begin() {
    Serial.begin(9600);
    addCommand("help", &printCommands);
    while (!Serial.available());
    Serial.read();
    delay(50);
}

void ArduinoConsole::printCommandList(ArduinoConsole *console) {
    Serial.println("Commands:");
    for (int i = 0; i < commandCount; ++i) {
        Serial.println(commands[i]);
    }
}

void ArduinoConsole::addCommand(const char *com, void (*f)(ArduinoConsole *)) {
    for (int i = 0; i < 20; ++i) {
        commands[commandCount][i] = com[i];
    }
    functions[commandCount] = f;
    commandCount++;
}

char *ArduinoConsole::getParameter(int pos) {
    if (pos > parametersCount) {
        return NULL;
    }
    return parameters[pos - 1];
}

bool ArduinoConsole::checkParameters(char *param, int pos) {
    int j = 0;
    if (pos != 0) {
        if (pos > parametersCount) { return false; }
        j = pos - 1;
    } else {
        pos = parametersCount;
    }
    for (; j < pos; ++j) {
        for (int i = 0; i < 20; ++i) {
            if (param[i] != parameters[j][i]) {
                if (parameters[j][i] == '=') {
                    i++;
                    for (int k = i; k < 20 - i; ++k) {
                        if (parameters[j][k] == '\0') {
                            return true;
                        }
                        param[k - i] = parameters[j][k];
                    }
                }
                break;
            }
            if (param[i] == '\0') {
                return true;
            }
        }
    }
    return false;
}

bool ArduinoConsole::Read(char *data, int &count, bool loop) {
    int n = -1;
    if (loop) {
        count = 0;
        while (n != 13) {
            if (n != -1) {
                if (n == 127 && count > 0) {
                    count--;
                    data[count] = 0;
                } else {
                    data[count] = (char) n;
                    count++;
                }
                Serial.print((char) n);
            }
            n = Serial.read();
        }
        Serial.println();
        data[count] = ' ';
        count++;
        return true;
    } else {
        n = Serial.read();
        if (n != -1) {
            if (n == 13) {
                Serial.println();
                data[count] = ' ';
                count++;
                return true;
            }
            if (n == 127 && count > 0) {
                count--;
                data[count] = 0;
            } else {
                data[count] = (char) n;
                count++;
            }
            Serial.print((char) n);
        }
    }
    return false;
}


void ArduinoConsole::run(bool loop) {
    if (enter) {
        enter = false;
        Serial.print("Arduino:\t");
    }
    if (!Read(serialInData, serialInCount, loop)) {
        return;
    }
    enter = true;
    parametersCount = 0;

    if (serialInCount != 0) {
        //get command
        int index = 0;
        char command[20] = {0};
        for (; index < 19 || index < serialInCount; ++index) {
            if (serialInData[index] == 32) {
                break;
            }
            command[index] = serialInData[index];
        }
        command[index] = 0;
        index++;

        //get parameters
        for (; parametersCount < 5; ++parametersCount) {
            while (serialInData[index] == 32) { index++; }
            if (index >= serialInCount) {
                break;
            }
            int i = 0;
            for (; i < 19 || index < serialInCount; ++index, ++i) {
                if (serialInData[index] == 32 || index >= serialInCount) {
                    break;
                }
                parameters[parametersCount][i] = serialInData[index];
            }
            parameters[parametersCount][i] = 0;
            index++;
        }

        int j = 0;
        for (; j < commandCount; ++j) {
            for (int i = 0; i < 20; ++i) {
                if (command[i] != commands[j][i]) {
                    for (int k = 0; k < 120; ++k) {
                        serialInData[k] = 0;
                    }
                    serialInCount = 0;
                    break;
                }
                if (command[i] == 0) {
                    functions[j](this);
                    for (int k = 0; k < 120; ++k) {
                        serialInData[k] = 0;
                    }
                    serialInCount = 0;
                    return;
                }
            }
        }

        Serial.print("No command found:");
        Serial.println(command);
    }
}

bool ArduinoConsole::getParameterAsByte(byte *data, int pos) {
    if (pos > parametersCount || pos <= 0) { return 0; }
    int count = 1;
    charToByte(parameters[pos], count, data);
    return 1;
}

int ArduinoConsole::getParameterAsInt(int pos) {
    if (pos > parametersCount || pos <= 0) { return 0; }
    return charToInt(parameters[pos - 1]);
}

void ArduinoConsole::charToByte(char *data, int &count, byte *out) {
    int index = 0;
    for (int i = 0; i < count; i += 2) {
        if (data[i] == '0' || data[i] == 'X' || data[i] == 'x' || data[i] == ' ' || data[i] == ',' || data[i] == 0) {
            continue;
        }
        for (int j = 0; j < 2; ++j) {
            if (j == 1 && data[i + j] != ' ' && data[i + j] != ',' && data[i + j] != 0) {
                out[index] *= 0x10;
            }
            switch (data[i + j]) {
                case '1':
                    out[index] += 0x01;
                    break;
                case '2':
                    out[index] += 0x02;
                    break;
                case '3':
                    out[index] += 0x03;
                    break;
                case '4':
                    out[index] += 0x04;
                    break;
                case '5':
                    out[index] += 0x05;
                    break;
                case '6':
                    out[index] += 0x06;
                    break;
                case '7':
                    out[index] += 0x07;
                    break;
                case '8':
                    out[index] += 0x08;
                    break;
                case '9':
                    out[index] += 0x09;
                    break;
                case 'a':
                case 'A':
                    out[index] += 0x0a;
                    break;
                case 'b':
                case 'B':
                    out[index] += 0x0b;
                    break;
                case 'c':
                case 'C':
                    out[index] += 0x0c;
                    break;
                case 'd':
                case 'D':
                    out[index] += 0x0d;
                    break;
                case 'e':
                case 'E':
                    out[index] += 0x0e;
                    break;
                case 'f':
                case 'F':
                    out[index] += 0x0f;
                    break;
                default:
                    break;
            }
        }
        index++;
    }
}

void ArduinoConsole::ReadAsByte(byte *data, int &count) {
    char text[120] = {0};
    Read(text, count);
    charToByte(text, count, data);
}

int ArduinoConsole::charToInt(char *data) {
    int out = 0;
    for (int k = 0; k < 20; ++k) {
        if (data[k] == 0) {
            break;
        }
        out *= 10;
        switch (data[k]) {
            case '1':
                out += 1;
                break;
            case '2':
                out += 2;
                break;
            case '3':
                out += 3;
                break;
            case '4':
                out += 4;
                break;
            case '5':
                out += 5;
                break;
            case '6':
                out += 6;
                break;
            case '7':
                out += 7;
                break;
            case '8':
                out += 8;
                break;
            case '9':
                out += 9;
                break;
            default:
                break;
        }
    }
    return out;
}

int ArduinoConsole::ReadAsInt() {
    int count = 0;
    char data[120] = {0};
    Read(data, count);
    return charToInt(data);
}





























