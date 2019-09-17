//================================================================================ library setup and initialization
#include <math.h>
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
int dfgh;
int refreshNum = 0;
int cutoffNum = 0;
const float voltagePin = A1;
const float currentPin = A0;
float voltageCalb = 1.25;
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
long wBeginTime;
long sBeginTime;
int startTime;
int endTime;
int frequency;
long sec = 0;
long mins = 0;
long hour = 0;
int ok = 0;
int setOption;
//======================================================================== listen to the ldrs
int listen(){
   int x = 0;
int y = 0;

//digitalWrite(11,LOW);
//Serial.println("enter the password");
  while(1){
 x = analogRead(A2);
 y = analogRead(A3);
 if(analogRead(A4)>5){
  ok = 1;
  digitalWrite(A5,HIGH);
  digitalWrite(0,HIGH);
  digitalWrite(1,HIGH);
  delay(100);
   digitalWrite(A5,LOW);
  digitalWrite(0,LOW);
  digitalWrite(1,LOW);
  return 0;
 }
 if(y>300&&y<500&&x<750&&x>550){
   delay(50);
 }
 else{
  if(x>800 && y>800){
    digitalWrite(A5,HIGH);
    delay(100);
    digitalWrite(A5,LOW);
    return 2;
    //Serial.println("2");
    
  }
    else if(x>800 && y<255){
      digitalWrite(1,HIGH);
       delay(100);
    digitalWrite(1,LOW);
    return 1;
    //Serial.println("1");
  }
     else if(x<200 && y<255){
      digitalWrite(0,HIGH);
       delay(100);
    digitalWrite(0,LOW);
    return 0;
    //Serial.println("0");
  }
 }
}
}

//======================================================================== set time for the wake up or sleeep of the system
void setTime(){
  while(1){
    delay(500);
     lcd.begin(16, 2);
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    int e = 0;
    lcd.setCursor(0,0);
    lcd.print("enter password");
     lcd.setCursor(10,1);
    lcd.print(a = listen());
    delay(300);
    lcd.setCursor(9,1);
    lcd.print(b = listen());
    delay(300);
    lcd.setCursor(8,1);
    lcd.print(c = listen());
    delay(300);
    lcd.setCursor(7,1);
    lcd.print(d = listen());
    delay(300);
    lcd.setCursor(6,1);
    lcd.print(e = listen());
    delay(300);
    if(a == 0&&b==1&&c==2&&d==2&&e==0){
      hour = 0;mins = 0;sec = 0;
      ok=0;
        lcd.clear();
       // lcd.begin(16, 2);
        lcd.setCursor(0, 0);
        // Print a message to the LCD.
        
        lcd.print("  welcome!");
        lcd.clear();
         lcd.setCursor(0,0);
         
         lcd.print("1.Set wake time");
         lcd.setCursor(0,1);
         lcd.print("2.Set sleep time");
         if(listen() == 1){
          lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("set wake time!!!") ;
        delay(1000);
        setOption = 1;
         }
         else if(listen() == 2){
          lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("set sleep time!!!") ;
        delay(1000);
        setOption = 2;
         }
        else{
          
          lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("wrong choice") ;
        delay(3000);
        break;
         
        }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("hrs:  min:  sec:") ;
       while(1){
        lcd.setCursor(0,1);
        
        lcd.print(hour+=int(listen()/2));
        //delay(100);
        if(ok == 1){
         
          break;
          
        }
         lcd.setCursor(6,1);
        
        lcd.print(mins+= listen()%2);
        //delay(100);
        if(ok == 1){
          
          break;
        }
           lcd.setCursor(11,1);
        
        lcd.print(sec+=int(pow(2,-listen())));
        //delay(100);
        if(ok == 1){
          
          break;
        }
        }
         if(setOption == 1){
            digitalWrite(10,LOW);
            digitalWrite(13,LOW);
            sBeginTime = 0;
          wBeginTime = millis();
          }
          else{
           digitalWrite(10,HIGH);
            digitalWrite(13,HIGH);
            cutoffNum = 4;  
            sBeginTime = millis();
            wBeginTime = 0;
          }
      break;      
    }
    else{
      lcd.clear();
        lcd.setCursor(0,0);
      lcd.print("wrong password");
      delay(1000);
      if(analogRead(A4)>5){
        break;
        }
      }
  }   
      lcd.clear(); 
      lcd.setCursor(0,0);
      lcd.print("Timer set for!"); 
      lcd.setCursor(0,1);
      lcd.print(hour*3600+mins*60+sec);
      lcd.setCursor(8,1);
      lcd.print("seconds!");
      delay(4000);
}
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

//======================================================================== thinSpeakRead
void thingSpeakRead(){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  espSerial.println(cmd);
  //if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    //if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  String talkBackCommand;
  char charIn;
  String talkBackURL =  "GET /channels/385007/fields/8/last?api_key=AHMMG7YLXARHC1UB&results=2\r\n" ;
  
  cmd = "AT+CIPSEND=";
  cmd += String(talkBackURL.length());
  espSerial.println(cmd);
  //if (DEBUG)  Serial.println(cmd);
  delay(100);
  if(1){
    espSerial.print(talkBackURL);
    //if (DEBUG)  Serial.print(talkBackURL);
  }
  else{
    espSerial.println("AT+CIPCLOSE");
   // if (DEBUG)   Serial.println("AT+CIPCLOSE");
    
  }
  if(analogRead(A4)>5){
        setTime();
      }
    delay(3000);
if(analogRead(A4)>5){
        setTime();
      }
   while (espSerial.available()) {
    charIn = (char)espSerial.read();
    talkBackCommand += charIn;
  }


  if(talkBackCommand[62] != '0' && talkBackCommand[61]=='0'&& talkBackCommand[60]==':'){
        cutoffNum = 1;
      }
  else if(talkBackCommand[62] != '0' && talkBackCommand[61]=='1'&& talkBackCommand[60]==':'&&cutoffNum != 2){
    //for(i = 0;i<2000;i++){
    digitalWrite(10,LOW);
    
   // }
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
 // if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    //if (DEBUG) Serial.println("AT+CIPSTART error");
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
  //if (DEBUG)  Serial.println(cmd);
  
  delay(100);
  if(espSerial.find(">")){
    espSerial.print(getStr);
    //if (DEBUG)  Serial.print(getStr);
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
  else{
    espSerial.println("AT+CIPCLOSE");
    // alert user
   // if (DEBUG)   Serial.println("AT+CIPCLOSE");
     lcd.begin(16, 2);
     lcd.print(" Reconnecting... ");
     connect_wifi();
    return false;
  }
  return true;
}



//================================================================================ LCD_refresh

void refresh(){
     if(wBeginTime!= 0){
      lcd.clear();
        lcd.setCursor(0, 0);
      lcd.print("wake time left: "); 
       lcd.setCursor(0, 1);
      lcd.print((hour*3600+mins*60+sec)-(millis()-wBeginTime)/1000); 
        lcd.setCursor(13, 1);
      lcd.print("sec"); 
      delay(2000);
     }
  
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
  
  espSerial.println("AT+CWMODE=1");   // set esp8266 as client
  delay(1000);
  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
  delay(5000);
         
 
}
//================================================================================ setup
void setup() {
   pinMode(A5, OUTPUT);
   pinMode(0, OUTPUT);
   pinMode(1, OUTPUT);
  pinMode(10, OUTPUT); 
  pinMode(11, OUTPUT); 
  pinMode(12, OUTPUT); 
  pinMode(13, OUTPUT); 
  digitalWrite(13,LOW);
   digitalWrite(1,LOW);
   digitalWrite(0,LOW);
   digitalWrite(10,LOW);
  espSerial.begin(115200);
  
   
  for(i = 0;i<2000;i++){
    
    InsCurrent += (analogRead(currentPin)-Ioffset);
    }
  Ioffset = InsCurrent/2000;  
  units = EEPROM_readDouble(0);
  lcd.begin(16, 2);
  lcd.print(" Connecting... ");
  // enable  serial
 //Serial.begin(9600);
  connect_wifi();
  refresh();

    }
//================================================================================ main program loop
void loop() 
{
  digitalWrite(12,HIGH);
 
  float SumVol = 0; 
  float SumCurr = 0;
  float numSam = 1;
  float sumVinsIins = 0;
  int flag = 0;
  frequency = 0;
    if(refreshNum>61){
    EEPROM_writeDouble(0,units);
    espSerial.println("AT+RST");         // Enable this line to reset the module;
    delay(1000);
    refreshNum = 0;
    }
    
    startTime = millis();
    while ((endTime-startTime) < 5000){
      if(analogRead(A4)>5){
        setTime();
        delay(500);
      }
      InsVoltage = analogRead(voltagePin);
      InsCurrent = -(analogRead(currentPin)-Ioffset);
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
     if(cutoffNum != 3){
       refresh();
       if(analogRead(A4)>5){
        setTime();
      }
       thingSpeakRead();
 
  }
     
     
     if(rmsVoltage > 300 || rmsCurrent > 20 || frequency > 100){
      cutoffNum=3 ;
      }

     // Serial.println(beginTime);
//      
      //Serial.println(millis());
     // Serial.println(cutoffNum);
     // Serial.println((hour*3600+mins*60+sec)*10000);
      if((millis()-sBeginTime) > (hour*3600+mins*60+sec)*1000 && sBeginTime!=0)
   {
    digitalWrite(10, LOW);
    digitalWrite(13,LOW);
    cutoffNum = 2;
    sBeginTime = 0;
    hour = 0;mins = 0;sec = 0;
   }
     if((millis()-wBeginTime) > (hour*3600+mins*60+sec)*1000 && wBeginTime!=0)
   {
   
    cutoffNum=3;
    wBeginTime = 0;
    hour = 0;mins = 0;sec = 0;
   }
     if(cutoffNum==1||cutoffNum==3||cutoffNum==4 ){
      
        digitalWrite(10, HIGH);
        
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
      else if(cutoffNum == 4){
        lcd.clear();
        lcd.setCursor(0, 0);
      lcd.print("sleep time left: "); 
       lcd.setCursor(0, 1);
      lcd.print((hour*3600+mins*60+sec)-(millis()-sBeginTime)/1000); 
        lcd.setCursor(13, 1);
      lcd.print("sec"); 
      }
      else
      {
       lcd.setCursor(2, 1);
      lcd.print(" Manual off "); 
      }
      delay(3000);
   }
   digitalWrite(11,LOW);
  
}
