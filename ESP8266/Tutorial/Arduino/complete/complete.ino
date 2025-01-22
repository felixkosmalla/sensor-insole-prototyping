


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
#define FORE_OUT_INDEX 1
#define HEEL_INDEX 3



// WiFi AP Settings
const char *ssid = "WearableTut";




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
  

  make_beep();
  
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


int previousHeelSensorValue = 0;
int previousForeInSensorValue = 0;


int heelThreshholdOn = 900;
int heelThreshholdOff = 200;

int foreInThreshholdOn = 800;
int foreInThreshholdOff = 200;



typedef enum {AIR, LANDING, TAKING_OFF} foot_state;
foot_state state = AIR;
foot_state prevState = AIR;

typedef enum {HEEL, MID_FORE} foot_strike;
foot_strike strike = MID_FORE;

bool heelSensorOn = false;
bool foreInSensorOn = false;


bool previousHeelSensorOn = false;
bool previousForeInSensorOn = false;




void loop() {
    webSocket.loop();
    server.handleClient();  
    beep_loop();


     // storing the old values in the corresponding variables
    previousHeelSensorValue = heelSensorValue;
    previousForeInSensorValue = foreInSensorValue;
    
    previousHeelSensorOn = heelSensorOn;
    previousForeInSensorOn = foreInSensorOn;
    
    prevState = state;



    
    foreInSensorValue =  ads.readADC_SingleEnded(FORE_IN_INDEX);      
    foreOutSensorValue =  ads.readADC_SingleEnded(FORE_OUT_INDEX);
    heelSensorValue =  ads.readADC_SingleEnded(HEEL_INDEX);



    // detection of heel and forefoot strike
    
    // which sensors are on (pressed) and which are not.
      if (heelSensorOn && heelSensorValue < heelThreshholdOff)
        heelSensorOn = false;
      else if (!heelSensorOn && heelSensorValue > heelThreshholdOn)
        heelSensorOn = true;

      if (foreInSensorOn && foreInSensorValue < foreInThreshholdOff)
        foreInSensorOn = false;
      else if (!foreInSensorOn && foreInSensorValue > foreInThreshholdOn)
        foreInSensorOn = true;

     
      // what is the current foot state
      if (state == AIR && (heelSensorOn || foreInSensorOn)) {
        
        state = LANDING;
        
      } else if (state == LANDING &&
                 (
                   (previousHeelSensorOn && !heelSensorOn) ||
                   (previousForeInSensorOn && !foreInSensorOn)                    
                 )
                ) {
        if (strike == HEEL){
          
          make_beep(200);
          
        }
        state = TAKING_OFF;
        
      } else if (!heelSensorOn && !foreInSensorOn && state != AIR) {
        if (strike == HEEL){
          make_beep(200);
          
          
        }
        state = AIR;
        
      }


      // detecting the heal strike
      if (state == LANDING && prevState != LANDING) {
        if (heelSensorOn && !foreInSensorOn) {
          if (strike == HEEL)
            make_beep(200);
          strike = HEEL;
          
        } else {
          strike = MID_FORE;
          
        }
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
