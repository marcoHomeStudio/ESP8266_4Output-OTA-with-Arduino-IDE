#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>

// Replace with your network credentials
const char* ssid = "***************";
  const char* password = "*************";
  const char* http_username = "admin";
  const char* http_password = "admin";
  // Create AsyncWebServer object on port 80
  AsyncWebServer server(80);
  // Create a WebSocket object  
  AsyncWebSocket ws("/ws");

  uint8_t output1 = D4;
  uint8_t output2 = D5;
  uint8_t output3 = D6;
  
  uint8_t output4 = D7;
// Initialize LittleFS
void initLittleFS() {
  LittleFS.begin();
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Serial.print("Connecting to WiFi ..");
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
      //Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      //Serial.printf("WebSocket client #%u disconnected\n", client->id());
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
  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);
  pinMode(output4, OUTPUT);
  initLittleFS();
  initWiFi();
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
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
  AsyncElegantOTA.begin(&server, http_username, http_password);

  // Start server
  server.begin();
}

void loop() {
  AsyncElegantOTA.loop();
  ws.cleanupClients();
}
