void notifyClients(String state) {
  ws.textAll(state);
}
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

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
