


#include <ESP8266WiFi.h>

extern "C" {
  #include "user_interface.h"
}

#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Adafruit_ADS1015.h>
#include <ArduinoJson.h>


#include <EEPROM.h>



int ledPin = LED_BUILTIN;

// where is the buzzer connected
#define BUZZER 14  

#define USE_SERIAL Serial


#define FORE_IN_INDEX 2
#define FORE_OUT_INDEX 3
#define HEEL_INDEX 1
#define HEEL_THRESHOLD_LOW 500
#define HEEL_THRESHOLD_HIGH 800
#define FORE_OUT_THRESHOLD_LOW 500
#define FORE_OUT_THRESHOLD_HIGH 900

bool HEEL_ON = false;
bool FORE_OUT_ON = false;
bool in_air = false;


// WiFi AP Settings
const char *ssid = "WearableTut2";




// initialize the webserver & websocket
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Analog to Digital Converter
Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */


bool is_beeping = false;
unsigned long beep_end = 0;

void make_beep(int duration){
    is_beeping = true;
    digitalWrite(BUZZER, HIGH);
    beep_end = millis() + duration;
    
}

void make_beep(){
  make_beep(1000); 
  
}

void beep_loop(){
  if(is_beeping){
    if (millis() > beep_end){
      is_beeping = false;
      digitalWrite(BUZZER, LOW);  
    }  
  }  
}


bool disable_wifi = false;





void setup() {
  delay(2000);
  Serial.begin(115200);
  Serial.setDebugOutput(1);
  
  pinMode(BUZZER, OUTPUT);
  beep_loop();


  ads.setGain(GAIN_TWOTHIRDS);  
  ads.begin();
  
 
  
 
  
  Serial.println();
  Serial.println("Configuring access point..."); 
  /* You can remove the password parameter if you want the AP to be open. */
  
  if(! disable_wifi){
    wifi_set_sleep_type(NONE_SLEEP_T);
  if( WiFi.softAP(ssid)){
      Serial.println("Start successfull!");
    }else{
      Serial.println("something went wrong");
    }

  IPAddress myIP = WiFi.softAPIP();
  Serial.println("AP IP address: ");
  Serial.println(myIP);
  server.on("/", _w_handleRoot);
  
 
  server.begin();
  Serial.println("HTTP server started");

  
 

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  }
  

  make_beep(100);
  
}





// the websocket is used to push sensor readings to a webfrontend
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            make_beep();
            make_beep();
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

                make_beep();
        
        // send message to client
        webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

            //send message to client
            webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);

            // send message to client 
            // webSocket.sendBIN(num, payload, length);
            break;
    }

}

unsigned long last_send = 0;


int16_t foreInSensorValue = 0; 
int16_t foreOutSensorValue = 0;
int16_t heelSensorValue = 0;






void loop() {
    webSocket.loop();
    server.handleClient();  
    beep_loop();



    
    foreInSensorValue =  ads.readADC_SingleEnded(FORE_IN_INDEX);      
    foreOutSensorValue =  ads.readADC_SingleEnded(FORE_OUT_INDEX);
    heelSensorValue =  ads.readADC_SingleEnded(HEEL_INDEX);

 /* if(heelSensorValue > HEEL_THRESHOLD_HIGH && FORE_OUT_ON == false && HEEL_ON == false){
    HEEL_ON = true;
    make_beep(500);
  }*/

  if(in_air && heelSensorValue > HEEL_THRESHOLD_HIGH){
    HEEL_ON = true;
    in_air = false;
    make_beep(500);
    }
/**

  if(foreOutSensorValue > FORE_OUT_THRESHOLD_HIGH){
    FORE_OUT_ON = true;

    if(heelSensorValue > HEEL_THRESHOLD_HIGH ){
      HEEL_ON = true;
    }else{
      HEEL_ON = false;
      }
      in_air = false;
  }

**/


  if(heelSensorValue < HEEL_THRESHOLD_LOW && foreOutSensorValue < FORE_OUT_THRESHOLD_LOW){
    FORE_OUT_ON = false;
    HEEL_ON = false;
    in_air = true;
    }

    
  if(in_air && foreOutSensorValue > FORE_OUT_THRESHOLD_HIGH){
    FORE_OUT_ON = true;
    in_air = false;    
  }


    // this is for the output to console and websocket
    if(last_send + 10 < millis()){
      last_send = millis();
      char buf [40];
      sprintf (buf, "%d,%d,%d",heelSensorValue, foreOutSensorValue, foreInSensorValue);
      Serial.println(buf);
      webSocket.broadcastTXT(buf); 
    }


     
    

}
