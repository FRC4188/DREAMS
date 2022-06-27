#include <SPI.h>
#include <SD.h>

#define SD_PIN 10
#define ERROR LED_BUILTIN

#define PRESSURE_PIN A0
#define LIGHT_PIN A1
#define HUMIDITY_PIN A3
#define TEMP_PIN A4


File file;
char filename[15];

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  pinMode(10, OUTPUT);
  
  initCard();
  createFile();
  openFile();
  initWrite();
}

void loop() {
  float time = float(millis()) / 1000;
  int pressure = analogRead(PRESSURE_PIN);
  int light = analogRead(LIGHT_PIN);
  int humidity = analogRead(HUMIDITY_PIN);
  int temperature = analogRead(TEMP_PIN);

  Serial.print(time, 2);
  Serial.print(",");
  Serial.print(pressure);
  Serial.print(",");
  Serial.print(light);
  Serial.print(",");
  Serial.print(humidity);
  Serial.print(",");
  Serial.println(temperature);

  file.print(time, 2);
  file.print(",");
  file.print(pressure);
  file.print(",");
  file.print(light);
  file.print(",");
  file.print(humidity);
  file.print(",");
  file.println(temperature);
  file.flush();
  
  delay(100);
}

void initCard() {
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_PIN)) {
    Serial.println("Card initialization failed");
    error(2);
  }
  Serial.println("Card initialization successful");
}

void createFile() {
  strcpy(filename, "MULLOG00.CSV");
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

  file.println("Time,Pressure,Light,Humidity,Temperature");
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
