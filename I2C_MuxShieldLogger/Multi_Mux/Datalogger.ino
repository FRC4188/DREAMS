#include <Wire.h>
#include <SD.h>

#define CHANNELS 16 
#define TOTAL_CHANNELS 48 
#define SD_PIN 10
#define ERROR LED_BUILTIN

File file;
char filename[15];

int vals[TOTAL_CHANNELS];

const int ids[] = {1, 2, 3};

void setup() {
    Serial.begin(9600);
    Wire.begin();

    pinMode(ERROR, OUTPUT);

    initCard();
    createFile();
    openFile();
    initWrite();
}

void loop() {
    for (int i = 0; i < sizeof(ids); i++) {
        int id = ids[i];
        Serial.print(id);
        Serial.print("|");

        file.print(id);
        file.print(",");

        for (int j = 0; j < 3; j++) {
            requestData(id, j);
            Wire.requestFrom(8, CHANNELS * 2);
            for (int k = 0; k < CHANNELS; k++) {
                int num = convertData();
                vals[k + (j * 16)] = num;
            }
        }

        float time = float(millis()) / 1000;

        Serial.print(time, 2);
        Serial.print(",");

        file.print(time, 2);
        file.print(",");

        for (int i = 0; i < TOTAL_CHANNELS; i++) {
            Serial.print(vals[i]);
            Serial.print(",");

            file.print(vals[i]);
            file.print(",");
        }
        Serial.println();
        file.println();

        file.flush();
    }
}

void requestData(int id, int row) {
    Wire.beginTransmission(id);
    Wire.write(row);
    Wire.endTransmission();
}

int convertData() {
    byte x;
    int j;
    int result;

    x = Wire.read();
    j++;

    if (j == 1) {
        result = (x << 8) | Wire.read();
        j = 0;
    }

    return result;
}

void initCard() {
    Serial.print("Initializing SD card...");

    if (!SD.begin(SD_PIN)) {
        Serial.println("Card initialization failed");
        error(2);
    }
}

void createFile() {
    strcpy(filename, "MUXLOG00.CSV");
    for (uint8_t i = 0; i < 100; i++) {
        filename[6] = '0' + i / 10;
        filename[7] = '0' + i % 10;
        // create if does not exist, do not open existing, write, sync after write
        if (!SD.exists(filename)) {
            break;
        }
    }
}

void openFile() {
    file = SD.open(filename, FILE_WRITE);

    if (!file) {
        Serial.print("Couldnt create ");
        Serial.println(filename);
        error(3);
    }
}

void initWrite() {
    Serial.print("Writing to ");
    Serial.println(filename);

    file.print("Id,Time,");
    for (int i = 0; i < TOTAL_CHANNELS; i++) {
        file.print("Chn");
        file.print(i);
        file.print(",");
    }
    file.println();
    file.flush();
}

void error(uint8_t errno) {
    while (1) {
        for (int i = 0; i < errno; i++) {
            digitalWrite(ERROR, HIGH);
            delay(100);
            digitalWrite(ERROR, LOW);
            delay(100);
        }
        for (int i = errno; i < 10; i++) {
            delay(200);
        }
    }
}
