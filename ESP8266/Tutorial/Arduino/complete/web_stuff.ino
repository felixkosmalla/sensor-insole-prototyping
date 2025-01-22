// triggers either EMS or Tone
void _w_trigger(){
   server.send(200, "text/html", "<h1>beep</h1>");
}

// disables or enables actuation
void _w_mute(){

    String message = "<h1>";

    server.send(200, "text/html", message);
 }




void _w_handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void _w_get_current_max(){
   
  
  }


// sets the maximum voluntary contraction. Parameter is 'max'
void _w_set_current_max(){
  bool did_set = false;
   for ( uint8_t i = 0; i < server.args(); i++ ) {
    if(server.argName(i) == "max"){
      //emg_max_value = server.arg(i).toInt(); 
      did_set = true;
     } 

    
  }
  
}


// sets the value when the user slips. parameter is 'min'
void _w_set_slip_min(){

}



void _w_params(){
  /**
  StaticJsonBuffer<600> jsonBuffer;

JsonObject& root = jsonBuffer.createObject();
root["emg_slip_value"] = emg_slip_value;
root["muted"] = muted;
root["emg_max_value"] = emg_max_value;

char result[600];
root.printTo(result);

server.send(200,"text/json", result);
  **/
}


void _w_set_params(){
  /**
  StaticJsonBuffer<600> jsonBuffer;
  
  if(server.args() > 0 ){
    JsonObject& root = jsonBuffer.parseObject(server.arg("params"));

    emg_slip_value = root["emg_slip_value"];
    emg_max_value = root["emg_max_value"];
    muted = root["muted"];
    Serial.println("muted");
    Serial.println(muted);

    
    server.send(200, "text/html", "alles ok");
      
  }else{
    server.send(500, "text/html", "no parameters supplied");
  }

  **/
   
   
  
}


