#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

/*
 *  ------Basic Date Logger, Luther Richardson --------
 *
 *  Explanation: Collect Analog data using the standard SD card library
 *
 *  
 */


// Variables
RTC_DS1307 RTC;
File logfile;

long totalseconds = 0;
long countPerMinute = 0;
long timePrevious = 0;
long timePreviousMeassure = 0;
long time = 0;
long countPrevious = 0;
float temp0,temp1,temp2;
float radiationValue = 0.0;
const int chipSelect = 10;
boolean serial = true;
boolean sd = true;
void setup(){
   Serial.begin(9600);
  


  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {

  }
  Serial.println("card initialized.");
  
  // create a new file and names it with a number in the file name
  char filename[] = "SDLOGGER.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  Wire.begin();  

  if (! logfile) {

  }
  logfile.println("hour,minute,seconds, seconds elasped,0,1,2,3,4,5");   
  Serial.print("Logging to: ");
  Serial.println(filename);










}

void loop(){
  // change the number after the greater than sign to change the data rate, 100 = 100 milliseconds
  if (millis()-timePreviousMeassure > 100){
    DateTime now;
    now = RTC.now();
    timePreviousMeassure = millis();
    if(serial){
      Serial.print("\n\n\n");
      Serial.print("Temp 0:\t");
      temp0 =  .878*(long)analogRead(0)-58.4;
      //Serial.print(temp0);
      Serial.print(analogRead(0));
      Serial.println();
      Serial.print("Temp 1:\t"); 
      temp1 =  .878*(long)analogRead(1)-58.4;
      //Serial.print(temp1);
      Serial.print(analogRead(1));
      Serial.println();
      Serial.print("Temp 2:\t"); 
      temp2 =  .878*(long)analogRead(2)-58.4;
      //Serial.print(temp2);
      Serial.print(analogRead(2));
      Serial.println();
      //Serial.print("Analog 3:\t"); 
      //Serial.print(analogRead(3));
      //Serial.println();
      //Serial.print("Analog 4:\t"); 
      //Serial.print(analogRead(4));
      //Serial.println();
      //Serial.print("Analog 5:\t"); 
      //Serial.print(analogRead(5));
      //Serial.println();
    } 
    if(sd){
      logfile.print(now.hour(), DEC);
      logfile.print(",");
      logfile.print(now.minute(), DEC);
      logfile.print(",");
      logfile.print(now.second(), DEC);
      logfile.print(',');
      logfile.print(totalseconds);
      logfile.print(',');
      logfile.print(temp0);
      logfile.print(",");
      logfile.print(temp1);
      logfile.print(",");
      logfile.print(temp2);
      //logfile.print(",");   
      //logfile.print(analogRead(3));
      //logfile.print(",");   
      //logfile.print(analogRead(4));
      //logfile.print(",");    
      //logfile.print(analogRead(5));
      //logfile.print(",");    
      logfile.println();  
      logfile.flush();
    }

    time++;


    totalseconds++;

    
  }

}


