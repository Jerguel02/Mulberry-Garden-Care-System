const int sensorIn = 34;      // pin where the OUT pin from sensor is connected on Arduino
int mVperAmp = 66;           // this the 5A version of the ACS712 -use 100 for 20A Module and 66 for 30A Module
int Watt = 0;
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
void setup() {
  Serial.begin (115200); 
  Serial.println ("ACS712 current sensor"); 
}
void loop()
{
Voltage = getVPP();
  VRMS = (Voltage/2.0) *0.707;   //root 2 is 0.707
  AmpsRMS = ((VRMS * 1000)/mVperAmp)-0.3; //0.3 is the error I got for my sensor
 
  Serial.println(AmpsRMS); 
  Serial.print(" Amps RMS  ---  ");
  Watt = (AmpsRMS*240/1.2);
  //delay(1000);
}
float getVPP()
{
  float result;
  int readValue;                // value read from the sensor
  int maxValue = 0;             // store max value here
  int minValue = 4096;          // store min value here ESP32 ADC resolution
  readValue = analogRead(sensorIn);
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * (3.3/4096.0)); //ESP32 ADC resolution 4096
      
   return result;
 }