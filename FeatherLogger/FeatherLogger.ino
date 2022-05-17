#include <SPI.h>
#include <SD.h>

#define SD_PIN 4

String FILE_NAME = "D34.csv";

void setup() {
    Serial.begin(9600);

    initSD();

    createFile();

    initWrite();
}

void loop() {
    File file = SD.open(FILE_NAME, FILE_WRITE);

    if (!file) {
        Serial.println("Cannot open file");
        while(1);
    }

    
}

void initSD() {
    Serial.print("Initializing SD card...");

    if (!SD.begin(SD_PIN)) {
        Serial.println("Card failed, or not present");
        while (1);
    }

    Serial.print("Card successfully initialized");
}

void createFile() {
    int i = 0;
    while (SD.exists(FILE_NAME)) {
        String beginning = "D34_";
        String end = ".csv";
        FILE_NAME = beginning + i + end;
        i++;
    }
}

void initWrite() {
    File file = SD.open(FILE_NAME, FILE_WRITE);
    file.print("Time, Accel X, Accel Y, Accel Z, Pressure, GPS");
    file.close();
}

float readPressure(uint8_t pin) {
    float raw = (float) analogRead(pin);
    return raw;
}










