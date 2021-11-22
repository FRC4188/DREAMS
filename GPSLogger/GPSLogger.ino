#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <RTClib.h>

// The serial connection from the GPS module is on pins 8 and 7.
SoftwareSerial mySerial(8, 7);

//Create logfile to save data.
File logfile;
// Record the starting time so that the RTC time is referenced to the start of the program.
DateTime startTime;

// Create an RTC object to more accurately record time.
RTC_Millis rtc;

#define LOG_INTERVAL  1000 // Milliseconds between entries.
#define ECHO_TO_SERIAL // Print everything logged to the Serial Monitor.
//#define WAIT_TO_START // Wait for sent data over Serial to begin logging.

#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

#define PMTK_Q_RELEASE "$PMTK605*31"

void setup() {
  Serial.begin(57600); // this baud rate doesn't actually matter!
  mySerial.begin(9600);
  delay(2000);
  Serial.println("Get version!");
  mySerial.println(PMTK_Q_RELEASE);

  // you can send various commands to get it started
  //mySerial.println(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  mySerial.println(PMTK_SET_NMEA_OUTPUT_ALLDATA);

  mySerial.println(PMTK_SET_NMEA_UPDATE_1HZ);

  
#ifdef WAIT_TO_START
  Serial.println("Send any character to start");
  while (!Serial.available());
#endif //WAIT_TO_START

  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
  
  if (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");

  // create a new file
  char filename[] = "GPSLOG00.txt";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);

  Wire.begin();

  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));

  // Set the startTime object to the current time at the beginning of the program.
  startTime = rtc.now().unixtime();
 }


void loop() {
  // Get the time at the loop.
  DateTime now = rtc.now().unixtime();

  // Relay any user input to the GPS Shield.
  if (Serial.available()) {
   char c = Serial.read();
   Serial.write(c);
   mySerial.write(c);
  }
  // If there is data coming in through the software serial, send it straight to the logFile and Serial connections.
  if (mySerial.available()) {
    char c = mySerial.read();
    logfile.print(c);
    // If the GPS data just ended a line, log the time of the next line.
    if (c == '\n') {
      logfile.print((now - startTime).seconds());
      logfile.print(",");
    }
    Serial.write(c);
  }
  logfile.flush();
}
