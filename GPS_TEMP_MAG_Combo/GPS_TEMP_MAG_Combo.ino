#include <Adafruit_GPS.h>
#include <Adafruit_MAX31865.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <RTClib.h>

// The serial connection from the GPS module is on pins 8 and 7.
SoftwareSerial mySerial(8, 7);

int chipselects[] = {2, 3, 4, 5}

// Parameters: (CS, DI, DO, CLK)
Adafruit_MAX31865 thermo1 = Adafruit_MAX31865(chipselects[0], 11, 12, 13);
Adafruit_MAX31865 thermo2 = Adafruit_MAX31865(chipselects[1], 11, 12, 13);
Adafruit_MAX31865 thermo3 = Adafruit_MAX31865(chipselects[2], 11, 12, 13);
Adafruit_MAX31865 thermo4 = Adafruit_MAX31865(chipselects[3], 11, 12, 13);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

//Create logfiles to save data.
File GPSLogfile;
File tempLogfile;

// Record the starting time so that the RTC time is referenced to the start of the program.
DateTime startTime;

// Create an RTC object to more accurately record time.
RTC_Millis rtc;

#define LOG_INTERVAL  1000 // Milliseconds between entries.

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
  char gpsFilename[] = "GPSLOG00.txt";
  for (uint8_t i = 0; i < 100; i++) {
    gpsFilename[6] = i/10 + '0';
    gpsFilename[7] = i%10 + '0';
    if (! SD.exists(gpsFilename)) {
      // only open a new file if it doesn't exist
      GPSLogfile = SD.open(gpsFilename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }

    // create a new file
  char tempFilename[] = "TPMLOG00.txt";
  for (uint8_t i = 0; i < 100; i++) {
    tempFilename[6] = i/10 + '0';
    tempFilename[7] = i%10 + '0';
    if (! SD.exists(tempFilename)) {
      // only open a new file if it doesn't exist
      tempLogfile = SD.open(tempFilename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  Serial.print("Logging Temperature/Magnetometer to: ");
  Serial.println(tempLogfile);

  tempLogfile.print("time,temp1,temp2,temp3,temp4,analog0,analog1,analog2\n");

  Wire.begin();

  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));

  // Set the startTime object to the current time at the beginning of the program.
  startTime = rtc.now().unixtime();

  thermo1.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
  thermo2.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
  thermo3.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
  thermo4.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
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
    GPSLogfile.print(c);
    // If the GPS data just ended a line, log the time of the next line.
    if (c == '\n') {
      GPSLogfile.print((now - startTime).seconds());
      GPSLogfile.print(",");
      
      tempLogfile.print((now - startTime).seconds());
      tempLogfile.print(",");
      tempLogfile.print(thermo1.readRTD());
      tempLogfile.print(",");
      tempLogfile.print(thermo2.readRTD());
      tempLogfile.print(",");
      tempLogfile.print(thermo3.readRTD());
      tempLogfile.print(",");
      tempLogfile.print(thermo4.readRTD());
      tempLogfile.print(",");
      tempLogfile.print(analogRead(A0));
      tempLogfile.print(",");
      tempLogfile.print(analogRead(A1));
      tempLogfile.print(",");
      tempLogfile.print(analogRead(A2));
      tempLogfile.print("\n");
    }
  }
  GPSLogfile.flush();
  tempLogfile.flush();
}
