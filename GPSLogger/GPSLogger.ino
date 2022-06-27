#include <SPI.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <avr/sleep.h>

SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define PRINT  true
/* set to true to only log to SD when GPS has a fix, for debugging, keep it false */
#define LOG_FIXONLY false

// Set the pins used
#define SD_PIN 10
#define ERROR LED_BUILTIN

File file;
char filename[15];

void setup() {
  Serial.begin(115200);
  
  pinMode(ERROR, OUTPUT);
  pinMode(10, OUTPUT);

  initCard();
  createFile();
  openFile();
}

void loop() {
  char c = GPS.read();

  #ifdef PRINT
    if (c) Serial.print(c);
  #endif

  if (GPS.newNMEAreceived()) {
    char *stringptr = GPS.lastNMEA();
    if (!GPS.parse(stringptr))   
      return;  

    if (LOG_FIXONLY && !GPS.fix) {
      Serial.print("No Fix");
      return;
    }

    uint8_t stringsize = strlen(stringptr);
    if (stringsize != file.write((uint8_t *)stringptr, stringsize)) {
        error(4);
    }

    if (strstr(stringptr, "RMC") || strstr(stringptr, "GGA")) {
      file.flush();
    }
    Serial.println();
  }
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
  strcpy(filename, "GPSLOG00.CSV");
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

void gpsConfig() {
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   
  GPS.sendCommand(PGCMD_NOANTENNA);

  Serial.println("GPS Configured");
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