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

#define output1  0
#define output2  2
#define output3  1
#define output4  3

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
  myArray["gpios"][0]["output"] = String(output1);
  myArray["gpios"][0]["state"] = String(digitalRead(output1));
  myArray["gpios"][1]["output"] = String(output2);
  myArray["gpios"][1]["state"] = String(digitalRead(output2));
  myArray["gpios"][2]["output"] = String(output3);
  myArray["gpios"][2]["state"] = String(digitalRead(output3));
  myArray["gpios"][3]["output"] = String(output4);
  myArray["gpios"][3]["state"] = String(digitalRead(output4));
  String jsonString;
  serializeJson(myArray, jsonString);
  return jsonString;
}

void notifyClients(String state) {
  ws.textAll(state);
}



void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void loadOutputFile(const char *outputFile){
  File ofile = LittleFS.open(outputFile, "r");
   String stringPayload;
  while (ofile.available()) {
    stringPayload+=(char)ofile.read();
  } 
   StaticJsonDocument<1024> outputGPIO;
   DeserializationError error = deserializeJson(outputGPIO, stringPayload.c_str());
   if (error){
    Serial.print(F("Failed to read output file, using default configuration "));
   }
   if (outputGPIO["gpios"][0]["state"]=="1"){digitalWrite(output1,HIGH);}
   if (outputGPIO["gpios"][0]["state"]=="0"){digitalWrite(output1,LOW);}
   if (outputGPIO["gpios"][1]["state"]=="1"){digitalWrite(output2,HIGH);}
   if (outputGPIO["gpios"][1]["state"]=="0"){digitalWrite(output2,LOW);}
   if (outputGPIO["gpios"][2]["state"]=="1"){digitalWrite(output3,HIGH);}
   if (outputGPIO["gpios"][2]["state"]=="0"){digitalWrite(output3,LOW);}
   if (outputGPIO["gpios"][3]["state"]=="1"){digitalWrite(output4,HIGH);}
   if (outputGPIO["gpios"][3]["state"]=="0"){digitalWrite(output4,LOW);}
   client1.publish(config.mqtt1Topic1,stringPayload.c_str());
   ofile.close();   
}

void loadConfiguration(const char *configFile, Config &config) {
  File file = LittleFS.open(configFile, "r");
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));
  config.init = doc["init"] | false;
  strlcpy(config.APname,                  // <- destination
          doc["APname"] | "Esp8266sensor",  // <- source
          sizeof(config.APname));         // <- destination's capacity
  strlcpy(config.APkey,
          doc["APkey"] | "123456789",
          sizeof(config.APkey));
  strlcpy(config.ESPhostname,
          doc["ESPhostname"] | "ESP4output",
          sizeof(config.ESPhostname));
  strlcpy(config.friendlyName,
          doc["friendlyName"] | "ESP8266 4 output module",
          sizeof(config.friendlyName));
  strlcpy(config.SSID,
          doc["SSID"] | "myApKey",
          sizeof(config.SSID));
  strlcpy(config.wifikey,
          doc["wifikey"] | "myWifiKey",
          sizeof(config.wifikey));
  strlcpy(config.username,
          doc["username"] | "admin",
          sizeof(config.username));
  strlcpy(config.password,
          doc["password"] | "admin",
          sizeof(config.password));
  strlcpy(config.mqtt1Server,
          doc["mqtt1Server"] | "pidev",
          sizeof(config.mqtt1Server));
  config.mqtt1Port=doc["mqtt1Port"] | 1883;
  strlcpy(config.mqtt1User,
          doc["mqtt1User"] | "admin",
          sizeof(config.mqtt1User));
  strlcpy(config.mqtt1Password,
          doc["mqtt1Password"] | "admin",
          sizeof(config.mqtt1Password));         
  strlcpy(config.mqtt1ClientId,
          doc["mqtt1ClientId"] | "DustSensor",
          sizeof(config.mqtt1ClientId));
  strlcpy(config.mqtt1Topic1,
          doc["mqtt1Topic1"] | "Esp8266Sensor/dustDensity",
          sizeof(config.mqtt1Topic1));
  strlcpy(config.mqtt1Topic2,
          doc["mqtt1Topic2"] | "Esp8266Sensor/stdDeviation",
          sizeof(config.mqtt1Topic2));
  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
}

void saveOutputFile(const char *outputFile, const char *payload){
  LittleFS.remove(outputFile);
  File file = LittleFS.open(outputFile, "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
  file.print((char*)payload);
  delay(1);
  //Close the file
  file.close();

}

void saveConfiguration(const char *configFile, const Config &config) {
  // Delete existing file, otherwise the configuration is appended to the file
  LittleFS.remove(configFile);
  // Open file for writing
  File file = LittleFS.open(configFile, "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<1024> doc;
  // Set the values in the document
  doc["init"] = config.init;
  doc["APname"]=config.APname;
  doc["APkey"]=config.APkey;
  doc["ESPhostname"] = config.ESPhostname;
  doc["friendlyName"] = config.friendlyName;
  doc["SSID"] = config.SSID;
  doc["wifikey"] = config.wifikey;
  doc["username"] = config.username;
  doc["password"] = config.password;
  doc["mqtt1Server"] = config.mqtt1Server;
  doc["mqtt1Port"] = config.mqtt1Port;
  doc["mqtt1Password"] = config.mqtt1Password;
  doc["mqtt1User"] = config.mqtt1User;
  doc["mqtt1ClientId"] = config.mqtt1ClientId;
  doc["mqtt1Topic1"] = config.mqtt1Topic1;
  doc["mqtt1Topic2"] = config.mqtt1Topic2;
  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}

void resetConfig(){
  config.init=true;
  strlcpy(config.password,  "12345", sizeof(config.password));
  strlcpy(config.username,  "admin", sizeof(config.username));
  strlcpy(config.SSID,  "<YOUR SSID>", sizeof(config.SSID));
  strlcpy(config.mqtt1Server,  "<YOUR MQTT SERVER>", sizeof(config.mqtt1Server));
  config.mqtt1Port=1883;
  strlcpy(config.mqtt1User,  "<YOUR MQTT USER>", sizeof(config.mqtt1User));
  strlcpy(config.mqtt1Password,  "<YOUR MQTT PASSWORD>", sizeof(config.mqtt1Password));
  strlcpy(config.mqtt1ClientId,  "<YOUR MQTT CLIENTID>", sizeof(config.mqtt1ClientId));
  strlcpy(config.mqtt1Topic1,  "<YOUR MQTT PUBLISH CHANNEL>", sizeof(config.mqtt1Topic1));
  strlcpy(config.mqtt1Topic2,  "<YOUR MQTT SUBSCRIBE CHANNEL>", sizeof(config.mqtt1Topic2)); 
  strlcpy(config.wifikey,  "<YOUR WIFI KEY>", sizeof(config.wifikey)); 
  strlcpy(config.ESPhostname,  "<DEVICE HOSTNAME>", sizeof(config.ESPhostname)); 
      
  saveConfiguration(configFile, config);
}

void printFile(const char *configFile) {
  // Open file for reading
  File file = LittleFS.open(configFile, "r");
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}

void callback1(char* topic, byte* payload, unsigned int length) {
  String stringPayload;
  for (int i = 0; i < length; i++) {
    stringPayload +=(char)payload[i];
  }
  notifyClients("{\"message\":\"Message has arrived\"}");
  notifyClients(stringPayload);
  Serial.print(F("MQTT message has arrived"));
  Serial.println(stringPayload);
  StaticJsonDocument<512> GPIO;
  DeserializationError error = deserializeJson(GPIO, payload);

  // Test if parsing succeeds.
  if (error) {
     notifyClients("{\"message\":\"deserializeJson() failed:"+ String(error.f_str())+ "\"}");
    return;
  }
  notifyClients(String(GPIO["gpios"]));
  if (GPIO["gpios"][0]["state"]=="1"){digitalWrite(output1,HIGH);}
  if (GPIO["gpios"][0]["state"]=="0"){digitalWrite(output1,LOW);}
  if (GPIO["gpios"][1]["state"]=="1"){digitalWrite(output2,HIGH);}
  if (GPIO["gpios"][1]["state"]=="0"){digitalWrite(output2,LOW);}
  if (GPIO["gpios"][2]["state"]=="1"){digitalWrite(output3,HIGH);}
  if (GPIO["gpios"][2]["state"]=="0"){digitalWrite(output3,LOW);}
  if (GPIO["gpios"][3]["state"]=="1"){digitalWrite(output4,HIGH);}
  if (GPIO["gpios"][3]["state"]=="0"){digitalWrite(output4,LOW);}
  client1.publish(config.mqtt1Topic1,stringPayload.c_str());
  saveOutputFile(outputFile,stringPayload.c_str());
  }

long lastReconnectAttempt = 0;
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    
    if (strcmp((char*)data, "states") == 0) {
      notifyClients(getOutputStates());
      String stringPayload=getOutputStates();
      char payload[stringPayload.length()+1];
      stringPayload.toCharArray(payload,sizeof(payload)+1 );
      saveOutputFile(outputFile,stringPayload.c_str());
      client1.publish(config.mqtt1Topic1,payload);      
    }
    else {
      int gpio = atoi((char*)data);
      digitalWrite(gpio, !digitalRead(gpio));
      notifyClients(getOutputStates()); 
      String stringPayload=getOutputStates();
      char payload[stringPayload.length()+1];
      stringPayload.toCharArray(payload,sizeof(payload)+1 );
      saveOutputFile(outputFile,stringPayload.c_str());  
      client1.publish(config.mqtt1Topic1,payload);
    }
  }
}
boolean reconnect() {
  if (client1.connect(config.mqtt1ClientId,config.mqtt1User,config.mqtt1Password)) {
  notifyClients("{\"message\":\"Connected to MQTT server\"}");  
  Serial.println(F("Connected to MQTT"));
  loadOutputFile(outputFile);
  printFile(outputFile); 

    client1.subscribe(config.mqtt1Topic2);
  }
  else {
    notifyClients("{\"message\":\"Can't connect to MQTT server\"}"); 
  }
  return client1.connected();
}
void setup() {
  
 initLittleFS();
 loadConfiguration(configFile, config);
 //Factory reset loop
  pinMode(3, FUNCTION_3);
  pinMode(3, INPUT_PULLUP);
  if(digitalRead(3)==LOW){
    bool factoryReset=HIGH;
    pinMode(1, FUNCTION_3);
    pinMode(1, OUTPUT);
    resetConfig();
    while(factoryReset==HIGH){
      digitalWrite(1,LOW);
      delay(1000);
      digitalWrite(1,HIGH);
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
  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  //Comment the following two lines to use the serial port
  pinMode(output3, OUTPUT);
  pinMode(output4, OUTPUT);
  
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }


 if(config.init==true){   
    WiFi.softAP(config.APname, config.APkey);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
     server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      if (!request->authenticate( config.username, config.password))
      {
        return request->requestAuthentication();
      }
       AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/setupinit.html","text/html");
        response->addHeader("username", config.username);
        response->addHeader("password", config.password);
        response->addHeader("ESPhostname", config.ESPhostname);
        response->addHeader("SSID", config.SSID);
        response->addHeader("APkey", config.wifikey);
        request->send(response);
        
      });
      server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/style.css", "text/css");
      });

      server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
         request->send(LittleFS, "/script.js", "text/javascript");
      });
      server.on("/config", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebParameter* p0 = request->getParam(0);
      strlcpy(config.password,  p0->value().c_str(), sizeof(config.password));
      AsyncWebParameter* p1 = request->getParam(1);
      strlcpy(config.SSID,  p1->value().c_str(), sizeof(config.SSID));
      AsyncWebParameter* p2 = request->getParam(2);
      strlcpy(config.wifikey,  p2->value().c_str(), sizeof(config.wifikey));
      AsyncWebParameter* p3 = request->getParam(3);
      strlcpy(config.ESPhostname,  p3->value().c_str(), sizeof(config.ESPhostname));
      request->redirect("/reboot");
    });
    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/reboot.html","text/html");
      response->addHeader("ESPhostname", config.ESPhostname);
      request->send(response);
      config.init=false;
      saveConfiguration(configFile, config);  
      restartMCU=true;
    });   
  }
else{
    initWiFi();
    initWebSocket();
    
    client1.setServer (config.mqtt1Server, config.mqtt1Port);
    client1.setCallback(callback1);
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      if (!request->authenticate(config.username, config.password))
        return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, "//index.html", "text/html");
      response->addHeader("ESPhostname", config.ESPhostname);
      response->addHeader("friendlyName", config.friendlyName);
        request->send(response);
    });
     server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/style.css", "text/css");
      });

      server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
         request->send(LittleFS, "/script.js", "text/javascript");
      });
    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(401);
    });
  
    server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/logged-out.html","text/html");
      response->addHeader("ESPhostname", config.ESPhostname);
      request->send(response);
    });
    server.serveStatic("/", LittleFS, "/");
    server.on("/setup", HTTP_GET, [](AsyncWebServerRequest * request) {
        if (!request->authenticate( config.username, config.password))
        {
          return request->requestAuthentication();
        }
        AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/setup.html", "text/html");
        response->addHeader("username", config.username);
        response->addHeader("password", config.password);
        response->addHeader("ESPhostname", config.ESPhostname);
        response->addHeader("friendlyName", config.friendlyName);
        response->addHeader("SSID", config.SSID);
        response->addHeader("APkey", config.wifikey);
        response->addHeader("mqtt1Server", config.mqtt1Server);
        response->addHeader("mqtt1Port", String(config.mqtt1Port));
        response->addHeader("mqtt1User", config.mqtt1User);
        response->addHeader("mqtt1Password", config.mqtt1Password);
        response->addHeader("mqtt1ClientId", config.mqtt1ClientId);
        response->addHeader("mqtt1Topic1", config.mqtt1Topic1);
        response->addHeader("mqtt1Topic2", config.mqtt1Topic2);
        request->send(response);
      });
    
    server.on("/config", HTTP_GET, [](AsyncWebServerRequest * request) {
      if (!request->authenticate( config.username, config.password))
      {
        return request->requestAuthentication();
      }
      
      AsyncWebParameter* p0 = request->getParam(0);
      strlcpy(config.password,  p0->value().c_str(), sizeof(config.password));
      AsyncWebParameter* p1 = request->getParam(1);
      strlcpy(config.SSID,  p1->value().c_str(), sizeof(config.SSID));
      AsyncWebParameter* p2 = request->getParam(2);
      strlcpy(config.wifikey,  p2->value().c_str(), sizeof(config.wifikey));
      AsyncWebParameter* p3 = request->getParam(3);
      strlcpy(config.ESPhostname,  p3->value().c_str(), sizeof(config.ESPhostname));
      AsyncWebParameter* p4 = request->getParam(4);
      strlcpy(config.friendlyName,  p4->value().c_str(), sizeof(config.friendlyName));
      AsyncWebParameter* p5 = request->getParam(5);
      strlcpy(config.mqtt1Server,  p5->value().c_str(), sizeof(config.mqtt1Server));
      AsyncWebParameter* p6 = request->getParam(6);
      strlcpy(config.mqtt1User,  p6->value().c_str(), sizeof(config.mqtt1User));
      AsyncWebParameter* p7 = request->getParam(7);
      strlcpy(config.mqtt1Password,  p7->value().c_str(), sizeof(config.mqtt1Password));
      AsyncWebParameter* p8 = request->getParam(8);
      strlcpy(config.mqtt1ClientId,  p8->value().c_str(), sizeof(config.mqtt1ClientId));
      AsyncWebParameter* p9 = request->getParam(9);
      strlcpy(config.mqtt1Topic1,  p9->value().c_str(), sizeof(config.mqtt1Topic1));
      AsyncWebParameter* p10 = request->getParam(10);
      strlcpy(config.mqtt1Topic2,  p10->value().c_str(), sizeof(config.mqtt1Topic2));
      request->redirect("/reboot");
    });
    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
      if (!request->authenticate( config.username, config.password))
      {
        return request->requestAuthentication();
      }
      request->send(LittleFS, "/reboot.html", "text/html");
      saveConfiguration(configFile, config); 
      restartMCU=true;
    });
    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
      if (!request->authenticate(config.username, config.password))
        return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/reboot.html","text/html");
      response->addHeader("ESPhostname", config.ESPhostname);
      request->send(response);
      resetConfig();
      restartMCU=true;
    });
    AsyncElegantOTA.begin(&server, config.username, config.password);  
  }
   server.begin(); 
  Serial.println(F("Print config file..."));
  printFile(configFile);
  
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
    }
  
}
