//---- program defs

#define FASTLED_ESP8266_RAW_PIN_ORDER

//---- headers

#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//------ WiFi
const char* ssid = "SmartDevices";
const char* pass = "dickface";

//------ FastLED defs

#define NUM_LEDS 50
#define DATA_PIN 2
//
#define CHIPSET WS2811
#define COLOR_ORDER BRG

byte red = 255;
byte green = 255;
byte blue = 255;
//
byte brightness = 255;

//
boolean stateOn = true;
int stateMode = 1;
int transitionTime = 0;

const int white = 1;
const int solid = 2;
const int rainbow = 3;
const int police = 4;
const int music = 5;

//
String effect_str = "Solid White";

// Other deps


//------        CODE      ------\\

WiFiClient espClient;
struct CRGB leds[NUM_LEDS];

extern void startMQTT();
extern void handleMQTT();
extern void startOTA();
extern void handleOTA();

//

void mqttReciever(char *topic, byte* payload, unsigned int length) {
  char msg[length+1]
  for(int i=0; i<length; i++){
    msg[i] = (char)payload[i];
  }
  msg[length] = '\0';
  
}

bool processJson(char* message) {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success()) {
    // we've failed
    return false;
  }
  
  // Set modes:
  if(root.containsKey("state")) {
    if(strcmp(root["state"],"on"== 0) {
      stateOn = true;
    } else if(strcmp(root["state"],"off"== 0) {
      stateOn = false;
    }
  }

  if(root.containsKey("effect")) {
    stateMode = root["effect"];
  }
  
  if(root.containsKey("rgb") {
    red = root["rgb"]["r"];
    green = root["rgb"]["g"];
    blue = root["rgb"]["b"];
    //
    calculateEffectColors();
    updateColors();
  }

  if(root.containsKey("brightness") {
    brightness = root["brightness"];
  }

  if(root.containsKey("transitionspeed")) {
    transitionTime = root["transitionspeed"];
  }
  
}

void showleds() {
  delay(1);
  //
  if (on) {
    FastLED.setBrightness(brightness);  //EXECUTE EFFECT COLOR
    FastLED.show();
    if (transitionTime > 0 && transitionTime < 130) {  //Sets animation speed based on receieved value
      FastLED.delay(1000 / transitionTime);
      //delay(10*transitionTime);
    }
  }
/*  else if (startFade) {
    setColor(0, 0, 0);
    startFade = false;
  } */
}

void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);  //for CYCLon
  }
}


// effects

void rainbow_beat() {
  uint8_t beatA = beatsin8(17, 0, 255);                        // Starting hue
  uint8_t beatB = beatsin8(13, 0, 255);
  fill_rainbow(leds, NUM_LEDS, (beatA + beatB) / 2, 8);        // Use FastLED's fill_rainbow routine.
  showleds();
}


//------        MAIN LOOP      ------\\

void setup() {
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,pass);
  //
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

boolean solid_color = false;
int oldMode = 0;
//
int oldRed = 0;
int oldGreen = 0;
int oldBlue = 0;
int oldBrightness = 0;
//
int realRed = 0;
int realGreen = 0;
int realBlue = 0;

void updateColor() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].red = realRed;
    leds[i].green = realGreen;
    leds[i].blue = realBlue;
  }
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void calculateEffectColors() {
  realRed = map(red,0,255,0, brightness);
  realGreen = map(green,0,255,0,brightness);
  realBlue = map(blue,0,255,0,brightness);
}

void loop() {
  // put your main code here, to run repeatedly:
  handleOTA();
  handleMQTT();
  
  //reset stuff on mode change:
  
  if(stateOn == true) {
    if(stateMode == 1) {
      if(solid_color == false || brightness != oldBrightness) {
        effect_str = "Solid White";
        red = 255;
        green = 255;
        blue = 255;
        oldBrightness = brightness;
        calculateEffectColors();
        updateColor();
        solid_color = true;
      }
    } else if(stateMode == 2) {
      if(solid_color == false || red != oldRed || green != oldGreen || blue != oldBlue || brightness != oldBrightness) {
        effect_str = "Custom Color";
        oldRed = red;
        oldGreen = green;
        oldBlue = blue;
        oldBrightness = brightness;
        //
        calculateEffectColors();
        updateColor()
        solidColor = true;
      }
    } else if(stateMode == 3) {
      if(oldMode != stateMode) {
        effect_str = "Rainbow";
      }
      rainbow_beat();
    }
  } else { // I guess we're off ;)
    fadeAll();
    showleds();
  }

  // Reset state
  if(oldMode != stateMode) {
    solid_color = false;
    oldMode = stateMode;
  }
}
