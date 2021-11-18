int32_t Htime;                    
int32_t Ltime;
float Ttime;
float frequency;
float capacitance;

void setup() {
  pinMode(8,INPUT);            //pin 8 as signal input

}

void loop() {
  for (int i=0;i<5;i++)        //measure time duration five times
    {
        Ltime=(pulseIn(8,HIGH)+Ltime)/2;        //get average for each cycle
        Htime=(pulseIn(8,LOW)+Htime)/2;
    }

    Ttime = Htime+Ltime;
    frequency=1000000/Ttime;
    
    capacitance = (1.44*1000000000)/(20800*frequency);   //calculating the Capacitance in nF
    Serial.println(capacitance);
    delay(500);

}






