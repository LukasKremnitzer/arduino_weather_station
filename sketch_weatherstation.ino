//SD-Card Library
#include <SPI.h>
#include <SD.h>

//DHT Temperature and Humidity Library
#include "DHT.h"

//Real Time Clock Library
#include <virtuabotixRTC.h>

//Define Pins for RTC
virtuabotixRTC myRTC(6, 7, 8);

//Define Pin and Sensor Type for DHT
#define DHTPIN 2
#define DHTSENSOR DHT11
DHT dht(DHTPIN, DHTSENSOR);

//Global Variables:
File Textdatei;
const int chipSelect = 10; //Pin for CS SD-Card Module
const int RecordTime = 5; //Recording Time in seconds for Wind speed
const int SensorPin = 3;  //Interrupt Pin for Wind speed
const float ConstForCalc = 2.4;  //Calculation constant for Wind speed
int InterruptCounter; //Counter rotations during RecordTime
float WindSpeed;  
float Temp;
float Humidity;

void setup() {
  // Initialize Serial
  Serial.begin(9600);
  while(!Serial);

  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  // Set once then comment out!!
  myRTC.setDS1302Time(00, 30, 10, 3 , 27, 10, 2021);
  
  //Initialze DHT Module
  dht.begin();

  //Initialize SD-Card Module
  Serial.println("Initialisiere SD-Karte");
  if (!SD.begin(chipSelect)){
    Serial.println("Initialisierung fehlgeschlagen!");
    while(true);
  }
  Serial.println("Initialisierung abgeschlossen");

  //Write Header (Column names) to Textfile
   Textdatei = SD.open("Test.txt", FILE_WRITE);
   if (Textdatei){
     Serial.println("Initialisiere Textdatei...");
     String headerString = "";
     headerString += "Temperatur";
     headerString += ";";
     headerString += "Luftfeuchtigkeit";
     headerString += ";";
     headerString += "Windgeschwindigkeit";
     headerString += ";";
     headerString += "Datetime";
     Textdatei.println(headerString);
     Textdatei.close();
     Serial.println("Abgeschlossen");
     }
   else{
     Serial.println("Textdatei konnte nicht geöffnet werden...");
   }
}

void loop() {
  measureWindspeed();
  Temp = dht.readTemperature();
  Humidity = dht.readHumidity();
  writeToSD();
  //readFromSD();
}

void measureWindspeed(){
  InterruptCounter = 0;
  attachInterrupt(digitalPinToInterrupt(SensorPin), countup, RISING);
  delay(1000 * RecordTime);
  detachInterrupt(digitalPinToInterrupt(SensorPin));
  WindSpeed = (float)InterruptCounter / (float)RecordTime * ConstForCalc;
}

void countup(){
  InterruptCounter++;
}

 void writeToSD(){
  // This allows for the update of variables for time or accessing the individual elements.
  myRTC.updateTime();
  String dateTimeString = "";
  dateTimeString += String(myRTC.year);
  dateTimeString += "-";
  dateTimeString += String(myRTC.month);
  dateTimeString += "-";
  dateTimeString += String(myRTC.dayofmonth);
  dateTimeString += " ";
  dateTimeString += String(myRTC.hours);
  dateTimeString += ":";
  dateTimeString += String(myRTC.minutes);
  dateTimeString += ":";
  dateTimeString += String(myRTC.seconds);
  
  Textdatei = SD.open("Test.txt", FILE_WRITE);
  if (Textdatei){
   Serial.println("Schreibe in Textdatei...");
   String dataString = "";
   dataString += String(Temp);
   dataString += ";";
   dataString += String(Humidity);
   dataString += ";";
   dataString += String(WindSpeed);
   dataString += ";";
   dataString += dateTimeString;
   Textdatei.println(dataString);
   Textdatei.close();
   Serial.println("Abgeschlossen");
   }
  else{
   Serial.println("Textdatei konnte nicht geöffnet werden...");
  }
}

// void readFromSD(){
//   Textdatei = SD.open("Test.txt");
//   if (Textdatei){
//     Serial.println("Wetterstations_Daten:");
//     while(Textdatei.available()){
//       Serial.write(Textdatei.read());
//     }
//     Textdatei.close();
//   }
//   else{
//     Serial.println("Textdatei konnte nicht geöffnet werden");
//   }
// }
