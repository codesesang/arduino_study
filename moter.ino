#include <SoftwareSerial.h>
#include <Servo.h>
#define DEBUG true

#define LEFTMOVE 20
#define RIGHTMOVE 21
#define DOOR 11
#define KITCHEN 12
#define ROOM 13

Servo mServo;
const int PIN_SERVO_PWM = 9;
int motestate=50;
 
SoftwareSerial esp8266(3,2); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                                        // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                                        // and the RX line from the esp to the Arduino's pin 3

void setup() {
  Serial.begin(9600);
  esp8266.begin(9600); // your esp's baud rate might be different

  mServo.attach(9);
  
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
  
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.println("test");
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CIOBAUD?\r\n",2000,DEBUG); // check baudrate (redundant)
  sendData("AT+CWMODE=1\r\n",1000,DEBUG); // configure as access point (working mode: AP+STA)
  //sendData("AT+CWLAP\r\n",3000,DEBUG); // list available access points
  sendData("AT+CWJAP=\"hisecN\",\"wjdqhqhdks\"\r\n",8000,DEBUG); // join the access point
  //sendData("AT+CWJAP=\"hyungjun2\",\"wjdqhqhdks\"\r\n",8000,DEBUG); // join the access point

 //sendData("AT+CIFSR=\"192.168.0.5\"\r\n",1000,DEBUG);
  //sendData("AT+CWDHCP=1,0\r\n",1000,DEBUG);
  //sendData("AT+CIPSTA=\"192.168.0.5\",\"255.255.255.0\",\"192.168.0.1\"\r\n",1000,DEBUG);

  
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
}
 
void loop() {
  if(esp8266.available()) { // check if the esp is sending a message
    if(esp8266.find("+IPD,")) {
      delay(1000); // wait for the serial buffer to fill up (read all the serial data)
      // get the connection id so that we can then disconnect
      int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
      esp8266.find("jun="); // advance cursor to "pin="
      int pinNumber = (esp8266.read()-48)*10; // get first number i.e. if the pin 13 then the 1st number is 1, then multiply to get 10
      pinNumber += (esp8266.read()-48); // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
      //digitalWrite(pinNumber, !digitalRead(pinNumber)); // toggle pin 
      Serial.println("---------");

     
     
      switch(pinNumber){
         
         
         case LEFTMOVE:
           Serial.println(pinNumber);
           if(motestate<30){ break; } // 더이상못움직임다
            else{
             motestate -= 25;
             mServo.write(motestate);
            }
           
         break;
         /////////////////////////////////////
         case RIGHTMOVE:
            Serial.println(pinNumber);
           if(motestate>360){ break; } //더이상 ㅁ소움직임다.오른쩍
            else{
             motestate += 25;
              mServo.write(motestate); 
            }

          break;

          
           //////////////////////////////////
          case DOOR:
            Serial.println(pinNumber);
            mServo.write(50);
            break;
          ///////////////////////////////////
          case KITCHEN:
            Serial.println(pinNumber);
            mServo.write(100);
            break;
           case ROOM:
             Serial.println(pinNumber);
             mServo.write(150);
             break;
            default: Serial.println("none..");
            break;
      }
      /*
 
     */
      // make close command
      String closeCommand = "AT+CIPCLOSE="; 
      closeCommand+=connectionId; // append connection id
      closeCommand+="\r\n";
      sendData(closeCommand,1000,DEBUG); // close connection
    }
  }
}
 
/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug) {
    String response = "";
    esp8266.print(command); // send the read character to the esp8266
    long int time = millis();
    
    while( (time+timeout) > millis()) {
      while(esp8266.available()) {
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }
    }
    
    if(debug) {
      Serial.print(response);
    }
    return response;
}

