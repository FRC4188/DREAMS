#include <SPI.h>
#include <SD.h>

#define chipSelect 10
File logfile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  pinMode(10, OUTPUT);
  
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  
  char filename[15];
  strcpy(filename, "ALLLOG00.CSV");
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = '0' + i/10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }
  
  logfile = SD.open(filename, FILE_WRITE);
  
  if( ! logfile ) {
    Serial.print("Couldnt create ");
    Serial.println(filename);
    while(1);
  }
  Serial.print("Writing to ");
  Serial.println(filename);

  logfile.println("Time, Pressure, Light, Humidity, Temperature");
  logfile.flush();
}

void loop() {
  Serial.print(millis()/ 1000.0, 2);
  Serial.print(",");
  Serial.print(analogRead(0));
  Serial.print(",");
  Serial.print(analogRead(1));
  Serial.print(",");
  Serial.print(analogRead(3));
  Serial.print(",");
  Serial.println(analogRead(4));

  logfile.print(millis() / 1000.0, 2);
  logfile.print(",");
  logfile.print(analogRead(0));
  logfile.print(",");
  logfile.print(analogRead(1));
  logfile.print(",");
  logfile.print(analogRead(3));
  logfile.print(",");
  logfile.println(analogRead(4));
  logfile.flush();
  
  delay(100);
}
