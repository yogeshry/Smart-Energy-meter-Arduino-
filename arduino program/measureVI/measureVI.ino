//================================================================================ library setup and initialization
#include "math.h"
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
SoftwareSerial espSerial =  SoftwareSerial(2,3);      // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to esp8266 module TX pin   -  connect the arduino TX pin to esp8266 module RX pin
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);                  // initialize the library with the numbers of the interface pins

//================================================================================ variable declaration
String apiKey = "36AP3UC6IHTEIV6R";                   // replace with your channel's thingspeak WRITE API key
String ssid="Connectify-yog";                                 // Wifi network SSID
String password ="123456780";                          // Wifi network password
boolean DEBUG=true;
int i;
int refreshNum = 0;
int cutoffNum = 0;
const float voltagePin = A1;
const float currentPin = A0;
float voltageCalb = 1;
float currentCalb = 18.9;
float powCalb = 0.92;
float Ioffset;
float unitconst = 3600000;
float InsVoltage ;
float InsCurrent = 0 ;
int rmsVoltage ;
float rmsCurrent ;
int realPower;
float pf;
float units = 0;
float cost = 0;
float rate = 7;
int startTime;
int endTime;
int frequency;

//======================================================================== EEPROM
void EEPROM_writeDouble(int ee, double value)
{
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++)
       EEPROM.write(ee++, *p++);
}

double EEPROM_readDouble(int ee)
{
   double value = 0.0;
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++)
       *p++ = EEPROM.read(ee++);
   return value;
}
//======================================================================== showResponce
void showResponse(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (espSerial.available()){
        c=espSerial.read();
        if (DEBUG) Serial.print(c);
      }
    }
                   
}
//======================================================================== thinSpeakRead
void thingSpeakRead(){
 
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  String talkBackCommand;
  char charIn;
  String talkBackURL =  "GET /channels/385007/fields/8/last?api_key=AHMMG7YLXARHC1UB&results=2\r\n" ;
  
  cmd = "AT+CIPSEND=";
  cmd += String(talkBackURL.length());
  espSerial.println(cmd);
  if (DEBUG)  Serial.println(cmd);
  delay(100);
  if(1){
    espSerial.print(talkBackURL);
    if (DEBUG)  Serial.print(talkBackURL);
    
  }
  else{
    espSerial.println("AT+CIPCLOSE");
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
    
  }
    delay(3000);

   while (espSerial.available()) {
    charIn = (char)espSerial.read();
    talkBackCommand += charIn;
  }

  Serial.print(talkBackCommand[53]);
  Serial.print(talkBackCommand[54]);
  Serial.print(talkBackCommand[55]);

  Serial.print(talkBackCommand[56]);
  Serial.print(talkBackCommand[57]);
  Serial.print(talkBackCommand[58]);
  Serial.print(talkBackCommand[59]);
  Serial.print(talkBackCommand[60]);
  Serial.print(talkBackCommand[61]);
   Serial.print(talkBackCommand[64]);
  Serial.print(talkBackCommand[65]);

  if(talkBackCommand[60] != '0' && talkBackCommand[59]=='0'&& talkBackCommand[58]==':'){
        cutoffNum = 1;
      }
  else if(talkBackCommand[60] != '0' && talkBackCommand[59]=='1'&& talkBackCommand[58]==':'&&cutoffNum != 2){
    for(i = 0;i<2000;i++){
    digitalWrite(10,LOW);
    }
    digitalWrite(13,LOW);
    cutoffNum = 2;
      }   
}

//========================================================================thingSpeakWrite
boolean thingSpeakWrite(int value1, float value2, int value3,float value4,float value5,float value6,int value7){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  
  
  String getStr = "GET /update?api_key=";   // prepare GET string
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(value1);
  getStr +="&field2=";
  getStr += String(value2);
  getStr +="&field3=";
  getStr += String(value3);
  getStr +="&field4=";
  getStr += String(value4);
  getStr +="&field5=";
  getStr += String(value5);
  getStr +="&field6=";
  getStr += String(value6);
  getStr +="&field7=";
  getStr += String(value7);
  getStr += "\r\n\r\n";
  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  espSerial.println(cmd);
  if (DEBUG)  Serial.println(cmd);
  
  delay(100);
  if(espSerial.find(">")){
    espSerial.print(getStr);
    
    if (DEBUG){  Serial.print(getStr);
    digitalWrite(11,HIGH);
    delay(70);
    digitalWrite(11,LOW);
    delay(70);
    digitalWrite(11,HIGH);
    delay(70);
    digitalWrite(11,LOW);
    delay(70);
    digitalWrite(11,HIGH);
    }
  }
  else{
    espSerial.println("AT+CIPCLOSE");
    // alert user
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
     lcd.begin(16, 2);
     lcd.print(" Reconnecting... ");
     connect_wifi();
    return false;
  }
  return true;
}



//================================================================================ LCD_refresh

void refresh(){
     
  
  // set up the LCD's number of columns and rows:
  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  // Print a message to the LCD.
  if(refreshNum % 2 == 0){
  lcd.print("Kwh:       pf:");
  lcd.setCursor(0,1);
  lcd.print("Rs :      ");
  lcd.setCursor(4, 0);
      lcd.print(units);
       lcd.setCursor(4,1);
      lcd.print(cost);
       lcd.setCursor(11,1);
      lcd.print(pf);
      }
  else{
  lcd.print("V:    V  P:    W");
  lcd.setCursor(0,1);
  lcd.print("I:    A  F:   Hz");
  lcd.setCursor(2, 0);
      lcd.print(rmsVoltage);
       lcd.setCursor(2,1);
      lcd.print(rmsCurrent);
       lcd.setCursor(11,0);
      lcd.print(realPower);
      lcd.setCursor(11,1);
      lcd.print(frequency);
  }
  
  
}
void connect_wifi(){
  DEBUG=true;           // enable debug serial
  espSerial.begin(115200);
//     espSerial.println("AT+RST");         // Enable this line to reset the module;
//    showResponse(1000);
  espSerial.println("AT+CWMODE=1");   // set esp8266 as client
  showResponse(1000);
  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
  showResponse(5000);
  if (DEBUG)  Serial.println("Setup completed");        
 
}
//================================================================================ setup
void setup() {
   pinMode(1, OUTPUT);
  pinMode(10, OUTPUT); 
  pinMode(11, OUTPUT); 
  pinMode(12, OUTPUT); 
  pinMode(13, OUTPUT); 
  digitalWrite(13,LOW);
   digitalWrite(1,LOW);
  for(i = 0;i<2000;i++){
    digitalWrite(10,LOW);
    InsCurrent += (analogRead(currentPin)-Ioffset);
    }
  Ioffset = InsCurrent/2000;  
  units = EEPROM_readDouble(0);
  lcd.begin(16, 2);
  lcd.print(" Connecting... ");
  // enable software serial
  Serial.begin(9600);
  connect_wifi();
  refresh();

    }
//================================================================================ main program loop
void loop() 
{
  digitalWrite(12, HIGH);
 
  float SumVol = 0; 
  float SumCurr = 0;
  float numSam = 1;
  float sumVinsIins = 0;
  int flag = 0;
  frequency = 0;
    if(refreshNum>61){
    EEPROM_writeDouble(0,units);
    espSerial.println("AT+RST");         // Enable this line to reset the module;
    showResponse(1000);
    refreshNum = 0;
    }
   digitalWrite(11,LOW); 
    startTime = millis();
    while ((endTime-startTime) < 5000){
      InsVoltage = analogRead(voltagePin);
      InsCurrent = (analogRead(currentPin)-Ioffset);
      //Serial.println(InsCurrent);
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
   //startTime = millis();
  
   rmsCurrent = 1.11*SumCurr/numSam/currentCalb;
   rmsVoltage = int(1.11*SumVol/numSam/voltageCalb);
   realPower = int(powCalb*sumVinsIins/numSam/voltageCalb/currentCalb);
   pf = realPower/rmsCurrent/rmsVoltage;
   units += realPower * 5/unitconst;
   frequency = frequency/5;
   cost = units * rate;
   if(pf > 1 || pf <-1){
    pf = 1;
   }
   //send data to ts
      if(refreshNum%2==0) {
           
           thingSpeakWrite(rmsVoltage,rmsCurrent,realPower,units,cost,pf,frequency);                                      // Write values to thingspeak
      }
    refreshNum++;
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
     Serial.print(" number of samples:  ");
     Serial.print(numSam);
     Serial.print(" units:  ");
     Serial.print(units);
     Serial.print(" cost:  ");
     Serial.print(Ioffset);
     Serial.println();
     if(cutoffNum != 3){
       refresh();
       thingSpeakRead();
  }
     
     if(rmsVoltage > 300 || rmsCurrent > 20 || frequency > 600){
      cutoffNum=3 ;
      }
     if(cutoffNum==1||cutoffNum==3 ){
      for(i = 0;i<1000;i++){
        digitalWrite(10, HIGH);
        }
        digitalWrite(13,HIGH);
      lcd.clear();
      lcd.begin(16, 2);
      lcd.setCursor(0, 0);
      lcd.print(" Emergency cut off ! ");
      if(rmsVoltage > 300)
      {
       lcd.setCursor(1, 1);
      lcd.print(" High voltage "); 
      }
      else if(rmsCurrent > 20)
      {
       lcd.setCursor(1, 1);
      lcd.print(" High Current "); 
      }
      else if(frequency > 60)
      {
       lcd.setCursor(0, 1);
      lcd.print(" High frequency "); 
      }
      else
      {
       lcd.setCursor(2, 1);
      lcd.print(" Manual off "); 
      }
      delay(3000);
   }

}
