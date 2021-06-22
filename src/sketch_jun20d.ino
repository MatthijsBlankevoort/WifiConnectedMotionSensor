#include <SoftwareSerial.h>
#define RX 2
#define TX 3
int ledPin = 13;             
int inputPin = 8;             
int pirState = LOW;    

String AP = "***"; // CHANGE ME
String PASS = "***"; // CHANGE ME
String deviceId = "***"; // CHANGE ME
String HOST = "api.pushingbox.com";
String uri = "http://api.pushingbox.com";
String PORT = "80";
int countTrueCommand; 
int countTimeCommand; 
boolean found = false; 
int valSensor = 0;
boolean requestSent = false; 
SoftwareSerial esp8266(RX,TX); 
 
  
void setup() {
  Serial.begin(9600);
  esp8266.begin(9600);
  pinMode(ledPin, OUTPUT);    
  pinMode(inputPin, INPUT);

  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
  getSensorData();
  if (valSensor == HIGH && !requestSent) {
    String getData = "GET /pushingbox?devid="+deviceId + " HTTP/1.1\r\n" ;
    getData+="Host: api.pushingbox.com\r\n";
    getData+="Connection: close\r\n\r\n";
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
    esp8266.println(getData);delay(1500);countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0",5,"OK");

    requestSent = true;
  }
 
}
void getSensorData(){  
  valSensor = digitalRead(inputPin);
  if (valSensor == HIGH) {        
    digitalWrite(ledPin, HIGH); 
    if (pirState == LOW) {
      Serial.println("Motion detected!");
      pirState = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW); 
    requestSent = false;
    if (pirState == HIGH){
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print("");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OK");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("ERROR");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
