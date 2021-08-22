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

void broadcastState(){
  String stringPayload=getOutputStates();
  char payload[stringPayload.length()+1];
  stringPayload.toCharArray(payload,sizeof(payload)+1 );
  Serial.println(stringPayload);
  saveOutputFile(outputFile,stringPayload.c_str());
  client1.publish(config.mqtt1Topic1,payload);      
}
