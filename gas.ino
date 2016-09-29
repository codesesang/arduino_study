//gas
#include <SoftwareSerial.h>

SoftwareSerial esp8266(3,2); // rx , tx

int mCount = 0;

String WebServerIP = "192.168.0.3";
String WebServerPort = "80";
int sensorValue = 0;
int flag = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  esp8266.begin(9600); 
  //Wificonnect("hyungjun2","wjdqhqhdks");
  Wificonnect("hisecN","wjdqhqhdks");
  Serial.println("is ok");
}

void loop() {
  if(esp8266.available()){
    if(esp8266.find("busy")){
      Serial.println("---------------------");
      Wificonnect("hisecN","wjdqhqhdks");
      flag=0;
    }
  }
  sensorValue = analogRead(1);           

  if(sensorValue > 300){
    Serial.println(sensorValue);
    SendDB(WebServerIP,"1",WebServerPort);
    flag=0;
    delay(3000);
    mCount++;
  }

  if(sensorValue < 300 && flag == 0){
    Serial.println(sensorValue);
    SendDB(WebServerIP,"0",WebServerPort); 
    flag=1;
    delay(3000);
    mCount++;
  }

  if(mCount==30){
    Wificonnect("hyungjun2","wjdqhqhdks");
    mCount=0;
  }

}

String SendData(String cmd , int timeout){
  String response = "";  
  esp8266.print(cmd);
  long int time = millis();
  while( (time + timeout) > millis() ){
    while(esp8266.available()){
      char c = esp8266.read();
      response += c;
    }
  }

  Serial.print(response);   
  return response;
}

void Wificonnect(String ssid, String pass){

  SendData("AT+RST\r\n",2000); //reset module
  SendData("AT+CIOBAUD?\r\n",2000); //check baudrate
  String cmd = "AT+CWJAP=\"";
  cmd += ssid;
  cmd += "\",\"";
  cmd += pass;
  cmd += "\"\r\n";
  
  SendData(cmd,8000); // join  AP
  SendData("AT+CIPMUX=1\r\n",1000); // configure for multiple connections
}

void SendDB(String ip, String value , String port){

  String cmd = "AT+CIPSTART=4,\"TCP\",\"";
  cmd += ip;
  cmd += "\",";
  cmd += port;
  cmd += "\r\n";
  
  SendData(cmd,1500);

  String post;

  post = "GET /Adata/gas.php?gas=";
  post += value;
  post += "\r\n\r\n";
  
  cmd = "AT+CIPSEND=4,";
  cmd += post.length();
  cmd += "\r\n";
    
  SendData(cmd,2500); // send 4 chnnal post byte >
  
  SendData(post,2000); //  > GET /Adata/gas.php?gas= 
  SendData("\r\n",1000);
  SendData("\r\n",1000);
  SendData("AT\r\n",1000);
}

