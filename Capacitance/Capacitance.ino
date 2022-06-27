#include <SD.h>

#define LOG_INTERVAL  500 // Milliseconds between entries.
#define ECHO_TO_SERIAL // Define to print all data to Serial.
//#define WAIT_TO_START // Define to wait for user input before logging.

int32_t Htime;                    
int32_t Ltime;
float Ttime;
float frequency;
float capacitance;

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

      // Write headings to the CSV file.
    logfile.println("sec,capacitance");
    #ifdef ECHO_TO_SERIAL
        Serial.println("sec,capacitance");
    #endif

    logfile.flush();
    
    pinMode(8,INPUT);            //pin 8 as signal input
}
void loop()
{

    // delay for the amount of time we want between readings
    delay(LOG_INTERVAL / 1000.0);
  
    // log milliseconds since starting
    uint32_t m = millis();
    logfile.print(m / 1000.0); // seconds since start
  
    for (int i=0;i<5;i++)        //measure time duration five times
    {
        Ltime=(pulseIn(8,HIGH)+Ltime)/2;        //get average for each cycle
        Htime=(pulseIn(8,LOW)+Htime)/2;
    }

    Ttime = Htime+Ltime;
    frequency=1000000/Ttime;
    
    capacitance = (1.44*1000000000)/(20800*frequency);   //calculating the Capacitance in nF

    logfile.print(",");
    logfile.println(capacitance);
    logfile.flush();

    Serial.println(capacitance);

}
