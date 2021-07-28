// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>


// Replace with your network credentials
const char* ssid = "****************";
const char* password = "******************";
const char* host = "esp8266-01";
const char* configfile = "/config.ini";
String configString;
String initval;
String sensorName;
String defaultAPkey;
String APname;
String wifikey;
String http_username;
String http_password;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

#define output1  0
#define output2  2
#define output3  1
#define output4  3



// Initialize LittleFS
void initLittleFS() {
  LittleFS.begin();
}
// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

String getOutputStates() {
  JSONVar myArray;
  myArray["gpios"][0]["output"] = String(output1);
  myArray["gpios"][0]["state"] = String(digitalRead(output1));
  myArray["gpios"][1]["output"] = String(output2);
  myArray["gpios"][1]["state"] = String(digitalRead(output2));
  myArray["gpios"][2]["output"] = String(output3);
  myArray["gpios"][2]["state"] = String(digitalRead(output3));
  myArray["gpios"][3]["output"] = String(output4);
  myArray["gpios"][3]["state"] = String(digitalRead(output4));
  String jsonString = JSON.stringify(myArray);
  return jsonString;
}

void notifyClients(String state) {
  ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  Serial.println((char*)data);
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "states") == 0) {
      notifyClients(getOutputStates());
    }
    else {
      int gpio = atoi((char*)data);
      digitalWrite(gpio, !digitalRead(gpio));
      notifyClients(getOutputStates());
    }
  }
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

void setup() {
  Serial.begin(115200);
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

  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);
  pinMode(output4, OUTPUT);
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  File f = LittleFS.open(configfile, "r");
  if (!f) {
    Serial.println("file open failed");
  }
  else
  {
    Serial.println("Reading config file:");
    //Data from file
    configString = f.readString();
    f.close();  //Close file
    Serial.println(configString);
    Serial.println("File Closed");
    deserialize();
  }
  initWiFi();
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username.c_str(), http_password.c_str()))
      return request->requestAuthentication();
    request->send(LittleFS, "/index.html", "text/html", false);
  });
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(401);
  });

  server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, "/logged-out.html", "text/html", false);
  });
  server.serveStatic("/", LittleFS, "/");

  // Start ElegantOTA
  AsyncElegantOTA.begin(&server, http_username.c_str(), http_password.c_str());

  // Start server
  server.begin();
}

void loop() {
  AsyncElegantOTA.loop();
  ws.cleanupClients();
}

void deserialize() {
  int marker1 = configString.indexOf("init:");
  int marker2 = configString.indexOf(",");
  initval = configString.substring(marker1 + 5, marker2);
  Serial.print("init:");
  Serial.println(initval);

  marker1 = configString.indexOf("sensorname:");
  marker2 = configString.indexOf(",", marker1 + 1);
  sensorName = configString.substring(marker1 + 11, marker2);
  Serial.print("sensorname:");
  Serial.println(sensorName);

  marker1 = configString.indexOf("defaultAPkey:");
  marker2 = configString.indexOf(",", marker1 + 1);
  defaultAPkey = configString.substring(marker1 + 13, marker2);
  Serial.print("defaultAPkey:");
  Serial.println(defaultAPkey);

  marker1 = configString.indexOf("SSID:");
  marker2 = configString.indexOf(",", marker1 + 1);
  APname = configString.substring(marker1 + 5, marker2);
  Serial.print("APname:");
  Serial.println(APname);

  marker1 = configString.indexOf("wifikey:");
  marker2 = configString.indexOf(",", marker1 + 1);
  wifikey = configString.substring(marker1 + 8, marker2);
  Serial.print("wifikey:");
  Serial.println(wifikey);

  marker1 = configString.indexOf("username:");
  marker2 = configString.indexOf(",", marker1 + 1);
  http_username = configString.substring(marker1 + 9, marker2);
  Serial.print("http_username:");
  Serial.println(http_username);

  marker1 = configString.indexOf("password:");
  marker2 = configString.indexOf(",", marker1 + 1);
  http_password = configString.substring(marker1 + 9, marker2);
  Serial.print("http_password:");
  Serial.println(http_password);


}
