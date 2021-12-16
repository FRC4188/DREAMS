#include <RTClib.h>
#include <SD.h>
#include <Wire.h>
#include <MuxShield.h>

#define CHANNELS 48 // Channels of data to log.
#define LOG_INTERVAL  1000 // Milliseconds between entries.
#define ECHO_TO_SERIAL // Define to print all data to Serial.
//#define WAIT_TO_START // Define to wait for user input before logging.

RTC_DS1307 RTC; // define the Real Time Clock object
MuxShield mux; // define the muxshield object

// use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;

// Report errors to the Serial Monitor and stop the program.
void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  
  while(1);
}

void setup()
{    
    Serial.begin(9600);
    
    #ifdef WAIT_TO_START
    Serial.println("Type any character to start");
    while (!Serial.available());
    #endif

    // initialize the SD card
    Serial.print("Initializing SD card...");
    // make sure that the default chip select pin is set to
    // output, even if you don't use it:
    pinMode(10, OUTPUT);

    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
    Serial.println("card initialized.");

    // create a new file
    char filename[] = "LOGGER00.CSV";
    for (uint8_t i = 0; i < 100; i++) {
        filename[6] = i/10 + '0';
        filename[7] = i%10 + '0';
        if (! SD.exists(filename)) {
            // only open a new file if it doesn't exist
            logfile = SD.open(filename, FILE_WRITE); 
            break;  // leave the loop!
        }
    }

    if (! logfile) {
      error("couldnt create file");
    }

    Serial.print("Logging to: ");
    Serial.println(filename);

    Wire.begin();

    if (!RTC.begin()) {
        logfile.println("RTC failed");
        #ifdef ECHO_TO_SERIAL
            Serial.println("RTC failed");
        #endif
    }
  
    // Write headings to the CSV file.
    logfile.print("sec");
    #ifdef ECHO_TO_SERIAL
        Serial.print("sec");
    #endif

    // Set up and add each input pin used to the header.
    for (int chn = 0; chn < CHANNELS; chn++) {
      logfile.print(",chn ");
      logfile.print(chn);
      mux.setMode(chn, ANALOG_IN);

      #ifdef ECHO_TO_SERIAL
        Serial.print(",chn ");
        Serial.print(chn);
      #endif
    }

    logfile.println();
    #ifdef ECHO_TO_SERIAL
      Serial.println();
    #endif

    logfile.flush();
}

void loop()
{
	DateTime now;

  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));

  // log milliseconds since starting
  uint32_t m = millis();
  logfile.print(m / 1000.0); // seconds since start
  #ifdef ECHO_TO_SERIAL
    Serial.print(m / 1000.0); // seconds since start
  #endif

  // fetch the time
  now = RTC.now();

  // Iterate through the channels.
  for (int chn = 0; chn < CHANNELS; chn++) {
    int data;
    logfile.print(",");
    #ifdef ECHO_TO_SERIAL
    Serial.print(",  ");
    #endif
    // Convert the channel numer to (IO,chn) and get the value.
    //data = mux.analogReadMS(chn / 16 + 1, chn % 16);
    data = analogRead(chn);
    logfile.print(data);
    #ifdef ECHO_TO_SERIAL
    Serial.print(data);
    #endif
  }
  logfile.println();
  #ifdef ECHO_TO_SERIAL
  Serial.println();
  #endif

  logfile.flush();
}
