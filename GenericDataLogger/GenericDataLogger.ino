#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

/*
 *  ------Geiger Tube board (Arduino Code) Example--------
 *
 *  Explanation: This example shows how to get the signal from the Geiger Tube
 *  in Arduino, we use one of the Arduino interrupt pins (PIN2).
 *  We count the time (ms) between two pulses of the Geiger tube.
 *
 *  Copyright (C) 2011 Libelium Comunicaciones Distribuidas S.L.
 *  http://www.libelium.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version:		0.3
 *  Design:		Marcos Yarza, David Gascon
 *  Implementation:	Marcos Yarza
 */




// Threshold values for the led bar
#define TH1 45
#define TH2 95
#define TH3 200
#define TH4 400
#define TH5 600

// Conversion factor - CPM to uSV/h

// Variables
RTC_DS1307 RTC;
File logfile;

long totalseconds = 0;
long countPerMinute = 0;
long timePrevious = 0;
long timePreviousMeassure = 0;
long time = 0;
long countPrevious = 0;
float radiationValue = 0.0;
const int chipSelect = 10;
boolean serial = true;
boolean sd = true;
void setup(){
   Serial.begin(19200);
  //pinMode(geiger_input, INPUT);


  //Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {

  }
  Serial.println("card initialized.");
  
  // create a new file
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
  if (millis()-timePreviousMeassure > 1000){
    DateTime now;
    now = RTC.now();
    timePreviousMeassure = millis();
    if(serial){
      Serial.print("\n\n\n");
      Serial.print("Analog 0:\t"); 
      Serial.print(analogRead(0));
      Serial.println();
      Serial.print("Analog 1:\t"); 
      Serial.print(analogRead(1));
      Serial.println();
      Serial.print("Analog 2:\t"); 
      Serial.print(analogRead(2));
      Serial.println();
      Serial.print("Analog 3:\t"); 
      Serial.print(analogRead(3));
      Serial.println();
      Serial.print("Analog 4:\t"); 
      Serial.print(analogRead(4));
      Serial.println();
      Serial.print("Analog 5:\t"); 
      Serial.print(analogRead(5));
      Serial.println();
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
      logfile.print(analogRead(0));
      logfile.print(",");
      logfile.print(analogRead(1));
      logfile.print(",");
      logfile.print(analogRead(2));
      logfile.print(",");   
      logfile.print(analogRead(3));
      logfile.print(",");   
      logfile.print(analogRead(4));
      logfile.print(",");    
      logfile.print(analogRead(5));
      logfile.print(",");    
      logfile.println();  
      logfile.flush();
    }

    time++;


    totalseconds++;

    
  }

}


