#include <RFduinoBLE.h>

// flag used to start sending
int flag = false;


int start;


const int  NUM_SENSORS = 3;
int STARTING_INDEX = 2;


int sensor_readings[NUM_SENSORS];



void RFduinoBLE_onConnect() {
  start = 0;
  flag = true;
  Serial.println("Device ist connnected.");
  Serial.println("Sending");
}

void RFduinoBLE_onDisconnect() {
  flag = false;
    Serial.println("Device ist disconnected.");

}





 void setup() {
   
  // this is the data we want to appear in the advertisement
  // (if the deviceName and advertisementData are too long to fix into the 31 byte
  // ble advertisement packet, then the advertisementData is truncated first down to
  // a single byte, then it will truncate the deviceName)
  RFduinoBLE.advertisementData = "Sole";
  RFduinoBLE.customUUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
  RFduinoBLE.deviceName = "Sole"; //Sets the device name to RFduino
//  RFduinoBLE.txPowerLevel = +20; //Sets the transmit power to max +4dBm

  // start the BLE stack
  RFduinoBLE.begin();
  analogReference(DEFAULT);
   
   
   
   
  //Wire.begin();
  Serial.begin(9600);
  Serial.println("Waiting for connection...");


  
}



void loop() {
  
  byte sendbuffer[2*NUM_SENSORS];
    
  
  for(int i = 0; i < NUM_SENSORS;i++){
     int val = analogRead(STARTING_INDEX + i);
     sensor_readings[i] = val; 
     
     
  
     sendbuffer[2*i] = val;
     sendbuffer[2*i + 1] = val >> 8;
     
     
  }
  Serial.println("---");
 
  
   
  if(flag) {
      RFduinoBLE.send((char*) sendbuffer, NUM_SENSORS  *2);
      // Sample at approx 30Hz
      delay(33);
  }
 

 
 
 
 }



