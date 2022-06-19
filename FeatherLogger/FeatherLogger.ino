

#include <SPI.h>
#include <SD.h>

#define SD_PIN 4
#define ACCELX_PIN A0
#define ACCELY_PIN A1
#define ACCELZ_PIN A2
#define PRESSURE_PIN A3

#define UNSET_PIN 5
#define SET_PIN 6

#define WROTE 8

#define TIME 13

float TRIGGER_PRESSURE = 0.3;//0.025; //atm

String FILE_NAME = "D34.csv";

void setup() {
    Serial.begin(9600);

    pinMode(ACCELX_PIN, INPUT);
    pinMode(ACCELY_PIN, INPUT);
    pinMode(ACCELZ_PIN, INPUT);
    pinMode(PRESSURE_PIN, INPUT);

    pinMode(WROTE, OUTPUT);
    pinMode(SET_PIN, OUTPUT);
    pinMode(UNSET_PIN, OUTPUT);

    digitalWrite(UNSET_PIN, HIGH);
    delay(100);
    digitalWrite(UNSET_PIN, LOW);

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

    int time = millis() / 1000;
    float accelx = readAccel(ACCELX_PIN);
    float accely = readAccel(ACCELY_PIN);
    float accelz = readAccel(ACCELZ_PIN);
    float pressure = readPressure(PRESSURE_PIN);

    

    // byPressure(pressure);
    byTime(time);

    Serial.print(time);
    Serial.print(", ");
    Serial.print(accelx, 3);
    Serial.print(", ");
    Serial.print(accely, 3);
    Serial.print(", ");
    Serial.print(accelz, 3);
    Serial.print(", ");
    Serial.println(pressure, 3);


    file.print(time);
    file.print(", ");
    file.print(accelx, 3);
    file.print(", ");
    file.print(accely, 3);
    file.print(", ");
    file.print(accelz, 3);
    file.print(", ");
    file.println(pressure, 3);

    file.close();

    digitalWrite(WROTE, HIGH);
    delay(1000);
    digitalWrite(WROTE, LOW);
}

void initSD() {
    Serial.print("Initializing SD card...");

    if (!SD.begin(SD_PIN)) {
        Serial.println("Card initialization failed, check card insertion");
        while (1);
    }

    Serial.print("Card initialization successful");
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
    file.println("Time, Accel X, Accel Y, Accel Z, Pressure, GPS");
    file.close();
}

float readPressure(uint8_t pin) {
    float raw = (float) analogRead(pin);
    float atm = 0.00112699 * raw - 0.152637; 
    return atm;
}

float readAccel(uint8_t pin) {
    return -0.500 * (float) analogRead(pin) + 262.500;
}


void byPressure(float pressure) {
    if (pressure < TRIGGER_PRESSURE) {
        digitalWrite(SET_PIN, HIGH);
        delay(100);
        digitalWrite(SET_PIN, LOW);
    } else {
        digitalWrite(UNSET_PIN, HIGH);
        delay(100);
        digitalWrite(UNSET_PIN, LOW);
    }
}

void byTime(int time) {
    timeIndicator(time);
    
    if (time == 120) {
        digitalWrite(SET_PIN, HIGH);
        delay(100);
        digitalWrite(SET_PIN, LOW);
        Serial.println("LAUNCH");
    } else if (time == 125) {
        digitalWrite(UNSET_PIN, HIGH);
        delay(100);
        digitalWrite(UNSET_PIN, LOW);
    }
}

void timeIndicator(int time) {
    if (time % 30 == 0) {
        digitalWrite(TIME, HIGH);
        delay(500);
        digitalWrite(TIME, LOW);
    }
} 
