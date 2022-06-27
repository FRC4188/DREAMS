#include<Wire.h>
#include <MuxShield.h>

#define ID 8
#define PRINT true

MuxShield muxShield;

void setup() {
    muxShield.setMode(1, ANALOG_IN);
    muxShield.setMode(2, ANALOG_IN);
    muxShield.setMode(3, ANALOG_IN);
    
    Serial.begin(9600);
    Wire.begin(ID);

    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
}

int vals[3][16];
int rows = sizeof(vals) / sizeof(vals[0]);
int cols = sizeof(vals[0]) / sizeof(vals[0][0]);

int row;

void loop() {
    #ifdef PRINT
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Serial.print(vals[i][j]);
            Serial.print(",");
        }
    Serial.print(" ");
    }
    Serial.println();
    #endif
}

void requestEvent() {
    readMux();
    sendData();
}

void readMux() {
    for (int i = 0; i < 16; i++) {
        vals[0][i] = muxShield.analogReadMS(1, i);
        vals[1][i] = muxShield.analogReadMS(2, i);
        vals[2][i] = muxShield.analogReadMS(3, i);
    }
}

void receiveEvent(int n) {
    row = Wire.read();
}

void sendData() {
    for (int j = 0; j < cols; j++) {
        int num = vals[row][j];
        Wire.write(highByte(num));
        Wire.write(lowByte(num));
    }
}