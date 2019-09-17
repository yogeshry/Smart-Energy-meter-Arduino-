#include "math.h"
const float voltagePin = A1;
const float currentPin = A0;
float voltageCalb = 0.93;
float currentCalb = 13.42;
float powCalb = 0.985;
float InsVoltage ;
float InsCurrent ;
float rmsVoltage ;
float rmsCurrent ;
float realPower;
float pf;
int startTime;
int endTime;

void setup() {
    Serial.begin(9600);
    }

void loop() 
{
float SumVol = 0; 
float SumCurr = 0;
float numSam = 0;
float sumVinsIins = 0;
int frequency = 0;
int flag = 0;
    startTime = millis();
    
    while ((endTime-startTime) < 13000) 
    {
      InsVoltage = analogRead(voltagePin); 
      InsCurrent = (analogRead(currentPin)-512);
      if(InsVoltage != 0)
        {
          flag = 0;
          numSam++;
          SumVol += InsVoltage;
          SumCurr += abs(InsCurrent);
          sumVinsIins += InsVoltage*InsCurrent;
        }
      else if(flag == 0)
        {
          frequency++;
          flag = 1;
        }
       endTime = millis();  
    }
 
  
   rmsCurrent = 1.11*SumCurr/numSam/currentCalb;
   rmsVoltage = 1.11*SumVol/numSam/voltageCalb;
   realPower = powCalb*sumVinsIins/numSam/voltageCalb/currentCalb;
   pf = realPower/rmsCurrent/rmsVoltage;
    
 Serial.print("rmsV: ");
 Serial.print(rmsVoltage);
 Serial.print("  rmsI: ");
 Serial.print(rmsCurrent);
 Serial.print("  RealPower: ");
 Serial.print(realPower);
 Serial.print("  pf: ");
 Serial.print(pf);
 Serial.print(" frequency:  ");
 Serial.print(frequency);
  Serial.print(numSam);
 Serial.println();


}
