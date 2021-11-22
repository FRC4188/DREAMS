// include the library code:
#include <LiquidCrystal.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(3,4,5,6,7,8);

// Create an RTC object to keep track of the time more accurately
RTC_Millis rtc;

//Create logfile to save data.
File logfile;

// Threshold values for the led bar
#define TH1 45
#define TH2 95
#define TH3 200
#define TH4 400
#define TH5 600

// Conversion factor - CPM to uSV/h
#define CONV_FACTOR 0.00812

#define LOG_INTERVAL  1000 // Milliseconds between entries.
#define ECHO_TO_SERIAL // Print everything logged to the Serial Monitor.
//#define WAIT_TO_START // Wait for sent data over Serial to begin logging.

// Variables
int ledArray [] = {10,11,12,13,9};
int geiger_input = 2;
long count = 0;
long countPerMinute = 0;
long timePrevious = 0;
long timePreviousMeassure = 0;
long time = 0;
long countPrevious = 0;
float radiationValue = 0.0;

DateTime startTime;

void setup(){
  pinMode(geiger_input, INPUT);
  digitalWrite(geiger_input,HIGH);
  for (int i=0;i<5;i++){
    pinMode(ledArray[i],OUTPUT);
  }

  Serial.begin(19200);

  //set up the LCD\'s number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Radiation Sensor");
  lcd.setCursor(0,1);
  lcd.print("Board - Arduino");  
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Cooking Hacks");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0,1);  
  lcd.print("www.cooking-hacks.com");
  delay(500);
  for (int i=0;i<5;i++){
    delay(200);  
    lcd.scrollDisplayLeft();
  }
  delay(500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  - Libelium -");
  lcd.setCursor(0,1);
  lcd.print("www.libelium.com");    
  delay(1000);

  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("CPM=");
  lcd.setCursor(4,0);
  lcd.print(6*count);
  lcd.setCursor(0,1);
  lcd.print(radiationValue);

  // The interrpt will listen for pulses, and record a count on a falling edge of a pulse.
  attachInterrupt(0,countPulse,FALLING);

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
  char filename[] = "GEIGER00.CSV";
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

  // Create CSV header.
  logfile.println("seconds,counts");
#ifdef ECHO_TO_SERIAL
  Serial.println("seconds,counts");
#endif

  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));

  startTime = rtc.now().unixtime();
}

void loop(){
  DateTime now;
  
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  
    countPerMinute = 60*count;
    radiationValue = countPerMinute * CONV_FACTOR;
    timePreviousMeassure = millis();
    lcd.clear();    
    lcd.setCursor(0, 0);
    lcd.print("CPM=");
    lcd.setCursor(4,0);
    lcd.print(countPerMinute);
    lcd.setCursor(0,1);
    lcd.print(radiationValue,4);
    lcd.setCursor(6,1);
    lcd.print(" uSv/h");

    //led var setting  
    if(countPerMinute <= TH1) ledVar(0);
    if((countPerMinute <= TH2)&&(countPerMinute>TH1)) ledVar(1);
    if((countPerMinute <= TH3)&&(countPerMinute>TH2)) ledVar(2);
    if((countPerMinute <= TH4)&&(countPerMinute>TH3)) ledVar(3);
    if((countPerMinute <= TH5)&&(countPerMinute>TH4)) ledVar(4);
    if(countPerMinute>TH5) ledVar(5);
    
      // log seconds since starting (as a double for decimals).
  now = rtc.now().unixtime();
  logfile.print((now - startTime).seconds());
  logfile.print(", ");    
#ifdef ECHO_TO_SERIAL
  Serial.print((now - startTime).seconds());
  Serial.print(", ");  
#endif

  // Log the Geiger count. Values will be processed after the flight.
  logfile.println(count);
#ifdef ECHO_TO_SERIAL
  Serial.println(count);
#endif

  logfile.flush();

  count = 0;
}

// This function will be called when a pulse is detected, and counts the pulses.
void countPulse(){
  detachInterrupt(0);
  count++;
  while(digitalRead(2)==0){
  }
  attachInterrupt(0,countPulse,FALLING);
}

// This function controls the LED array on the Geiger counter shield.
void ledVar(int value){
  if (value > 0){
    for(int i=0;i<=value;i++){
      digitalWrite(ledArray[i],HIGH);
    }
    for(int i=5;i>value;i--){
      digitalWrite(ledArray[i],LOW);
    }
  }
  else {
    for(int i=5;i>=0;i--){
      digitalWrite(ledArray[i],LOW);
    }
  }
}
