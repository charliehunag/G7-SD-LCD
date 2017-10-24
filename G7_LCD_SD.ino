#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include<LiquidCrystal_I2C.h>
#include<Wire.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
File myFile;

#define RxD 2
#define TxD 3
SoftwareSerial Serial1(RxD,TxD);

long pmcf10=0;
long pmcf25=0;
long pmcf100=0;
long pmat10=0;
long pmat25=0;
long pmat100=0;

char buf[50];

void setup() {
  // put your setup code here, to run once:
 
  Serial.begin(9600);
  Serial1.begin(9600);
  lcd.begin(16, 2);           
  lcd.backlight();               
  SD.begin(4);
  Serial.println("IR test!");
  Serial.print("Initializing SD card...");

  if(!SD.begin(4)){
    Serial.println("SD card failed!");
    return;
  }
  Serial.println("SD card success!");

  if(SD.exists("datalog.txt")){
    Serial.println("datalog.txt exists.");
  }
  else{
    Serial.println("datalog.txt not exists.");
  }

  Serial.println("Creating datalog.txt...");


  myFile=SD.open("datalog.txt",FILE_WRITE);

}

void loop() {
 
  int count = 0;
  unsigned char c;
  unsigned char high;
  
  while (Serial1.available()) {
    c = Serial1.read();
    if((count==0 && c!=0x42) || (count==1 && c!=0x4d)){
      Serial.println("check failed");
      break;
    }
    if(count > 15){
      Serial.println("complete");
      break;
    }
    else if(count == 4 || count == 6 || count == 8 || count == 10 || count == 12 || count == 14) {
      high = c;
    }
    else if(count == 5){
      pmcf10 = 256*high + c;
/*      Serial.print("CF=1, PM1.0=");
      Serial.print(pmcf10);
      Serial.println(" ug/m3");*/
    }
  else if(count == 7){
     pmcf25 = 256*high + c;
/*    Serial.print("CF=1, PM2.5=");
      Serial.println("");
      Serial.println(pmcf25);
      Serial.println(" ug/m3");*/
    }
  else if(count == 9){
      pmcf100 = 256*high + c;
/*      Serial.print("CF=1, PM10=");
      Serial.print(pmcf100);
      Serial.println(" ug/m3");*/
    }
    else if(count == 11){
      pmat10 = 256*high + c;
/*       Serial.print("atmosphere, PM1.0=");
      Serial.print(pmat10);
      Serial.println(" ug/m3");*/
    }
    else if(count == 13){
      pmat25 = 256*high + c;
      Serial.print("atmosphere, PM2.5=");
      Serial.println("");
      Serial.println(pmat25);
      Serial.println(" ug/m3");
      lcd.setCursor(0,0);
      lcd.print(pmat25);
      lcd.print("ug/m3");
    }
    else if(count == 15){
      pmat100 = 256*high + c;
      Serial.print("atmosphere, PM10=");
      Serial.println("");
      Serial.println(pmat100);
      Serial.println(" ug/m3");
    }
    count++;
  }
  while(Serial1.available()) Serial1.read();
  Serial.println();
  
  myFile=SD.open("datalog.txt",FILE_WRITE);
  
  if(myFile){
    myFile.print("atmosphere, PM2.5=");
    myFile.println("");
    myFile.println(pmat25);
    myFile.println(" ug/m3");
    myFile.print("atmosphere, PM10=");
    myFile.println("");
    myFile.println(pmat100);
    myFile.println(" ug/m3");
    myFile.println("complete");
    myFile.println("");
    myFile.close();
  }
  else{
    Serial.println("error in opening datalog.txt");
  }
  delay(10000);
}

