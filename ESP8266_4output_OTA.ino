// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <ArduinoJson.h>
#include <AsyncElegantOTA.h>
#include <PubSubClient.h>

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

#define output0  0
#define output1  2
#define output2  1
#define output3  3
#define outputLed 1
#define factoryResetButton 3

struct Config {
  bool init;
  char APname[64];
  char APkey[64];
  char ESPhostname[64];
  char friendlyName[64];
  char SSID[64];
  char wifikey[64];
  char username[64];
  char password[64];
  char mqtt1Server[64];
  int mqtt1Port;
  char mqtt1User[64];
  char mqtt1Password[64];
  char mqtt1ClientId[64];
  char mqtt1Topic1[64];
  char mqtt1Topic2[64];  
};

const char *configFile = "/config.json";
const char *outputFile = "/output.json";
Config config;
bool restartMCU=false;
WiFiClient espClient;
PubSubClient client1(espClient);
unsigned long broadcastStateTimer=60000;
unsigned long broadcastStateTime=0;
// Initialize LittleFS
void initLittleFS() {
  LittleFS.begin();
}
// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.hostname(config.ESPhostname);
  WiFi.begin(config.SSID, config.wifikey);
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

String getOutputStates() {
  DynamicJsonDocument myArray(1024);
  myArray["output0"] = String(digitalRead(output0));
  myArray["output1"] = String(digitalRead(output1));
  myArray["output2"] = String(digitalRead(output2));
  myArray["output3"] = String(digitalRead(output3));
  String jsonString;
  serializeJson(myArray, jsonString);
  return jsonString;
}

long lastReconnectAttempt = 0;

void setup() {
  
 initLittleFS();
 loadConfiguration(configFile, config);
 //Factory reset loop
  pinMode(factoryResetButton, FUNCTION_3);
  pinMode(factoryResetButton, INPUT_PULLUP);
  if(digitalRead(factoryResetButton)==LOW){
    bool factoryReset=HIGH;
    pinMode(outputLed, FUNCTION_3);
    pinMode(outputLed, OUTPUT);
    resetConfig();
    while(factoryReset==HIGH){
      digitalWrite(outputLed,LOW);
      delay(1000);
      digitalWrite(outputLed,HIGH);
      delay(1000);
    }
  }
  
  
  //********** CHANGE PIN FUNCTION  TO GPIO **********
  //GPIO 1 (TX) swap the pin to a GPIO.
  pinMode(1, FUNCTION_3);
  //GPIO 3 (RX) swap the pin to a GPIO.
  pinMode(3, FUNCTION_3);
  //**************************************************
 
  /********** CHANGE PIN FUNCTION  TO TX/RX **********
    //GPIO 1 (TX) swap the pin to a TX.
    pinMode(1, FUNCTION_0);
    //GPIO 3 (RX) swap the pin to a RX.
    pinMode(3, FUNCTION_0);
    //***************************************************
    */
  Serial.begin(115200);
  Serial.println("Starting device...");
  pinMode(output0, OUTPUT);
  pinMode(output1, OUTPUT);
  //Comment the following two lines to use the serial port
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);
  
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }


 if(config.init==true){   
    WiFi.softAP(config.APname, config.APkey);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    homepageini();
    stylecss();
    script();
    configini();
    rebootini();  

    
  }
else{
    initWiFi();
    initWebSocket();    
    client1.setServer (config.mqtt1Server, config.mqtt1Port);
    client1.setCallback(callback1);
    
    // Route for root / web page
    homepage();
    logout();
    loggedout();
    server.serveStatic("/", LittleFS, "/");
    setuppage();
    configpage();
    reboot();
    resetpage();
    AsyncElegantOTA.begin(&server, config.username, config.password);  
  }
  server.begin(); 
  Serial.println(F("Print config file..."));
  printFile(configFile);
  broadcastStateTime=millis();  
  lastReconnectAttempt = 0;
}
void loop() {
  if (restartMCU==true){
    delay(1000);
    ESP.restart();
  }
   if (config.init==false){
    AsyncElegantOTA.loop();
    if (!client1.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    client1.loop();
  }
    ws.cleanupClients();
    if (millis()-broadcastStateTime>=broadcastStateTimer){
    broadcastState();
    broadcastStateTime=millis();
   }
  }
  
}
