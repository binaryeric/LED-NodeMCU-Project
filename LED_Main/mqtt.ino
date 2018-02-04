//------      MQTT SERVER     ------\\

//------ headers
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

//------ defs
const char* mqtt_server = "192.168.0.175";
const int mqtt_port = 8080;

#define sensor_name "Wall LEDs"
#define OTApass "password"
int OTAport = 8081;

//----- MQTT topics
const char* light_state_topic = "eric/room";
const char* light_set_topic = "eric/room/set";

const char* on_cmd = "ON";
const char* off_cmd = "OFF";
const char* effect = "solid";
String effectString = "solid";
String oldeffectString = "solid";

//---- MQTT packets


//------        CODE      ------\\

extern WiFiClient espClient;
PubSubClient client(espClient);
extern boolean processJson();

void callback(char *topic, byte* payload, unsigned int length) {
  char msg[length+1];
  for(int i=0; i<length; i++){
    msg[i] = (char)payload[i];
  }
  msg[length] = '\0';

  if(processJson(msg)) {
    
  } else {
    // nothing
  }
  
}

void startMQTT() {
  client.setServer(mqtt_server,mqtt_port);
  client.setCallback(callback);
}

void connectMQTT() {
  while(!client.connected()) {
    if(client.connect(sensor_name)) {
      client.subscribe(light_set_topic);
    }
  }
}

void handleMQTT() {
  client.loop();
}

extern boolean stateOn;
extern byte red;
extern byte green;
extern byte blue;
extern int stateMode;
extern int transitionTime;
extern String effect_str;

//
void sendMQTT() {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root["state"] = (stateOn) ? "on" : "off";
  JsonObject& color = root.createNestedObject("color");
  color["r"] = red;
  color["g"] = green;
  color["b"] = blue;

  root["brightness"] = brightness;
  root["effect"] = effectString.c_str();


  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  client.publish(light_state_topic, buffer, true);
}

//------ OTA
void startOTA() {

  ArduinoOTA.setPort(OTAport);
  ArduinoOTA.setHostname(sensor_name);
  ArduinoOTA.setPassword((const char*)OTApass);
  //
  /*
  ArduinoOTA.onStart([]() {
    // starting
  }
  ArduinoOTA.onEnd([]() {
    //Serial.println("\nEnd");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
   // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
/*    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin(); */
}


void handleOTA() {
  ArduinoOTA.handle();
}

