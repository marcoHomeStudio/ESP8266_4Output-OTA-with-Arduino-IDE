void homepageini(){
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
}

void homepage(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      if (!request->authenticate(config.username, config.password))
        return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, "//index.html", "text/html");
      response->addHeader("ESPhostname", config.ESPhostname);
      response->addHeader("friendlyName", config.friendlyName);
      response->addHeader("output0Friendlyname", config.output0Friendlyname);
      response->addHeader("output0reverse", String(config.output0reverse));
      response->addHeader("output1Friendlyname", config.output1Friendlyname);
      response->addHeader("output1reverse", String(config.output1reverse));
      response->addHeader("output2Friendlyname", config.output2Friendlyname);
      response->addHeader("output2reverse", String(config.output2reverse));
      response->addHeader("output3Friendlyname", config.output3Friendlyname);
      response->addHeader("output3reverse", String(config.output3reverse));
        request->send(response);
    });
}

void stylecss(){
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(LittleFS, "/style.css", "text/css");
  });
}

void script(){
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(LittleFS, "/script.js", "text/javascript");
  });
}

void configini(){
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
}

void rebootini(){
  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/rebootini.html","text/html");
    response->addHeader("ESPhostname", config.ESPhostname);
    request->send(response);
    config.init=false;
    saveConfiguration(configFile, config);  
    restartMCU=true;
  });   
}

void reboot(){
    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
      if (!request->authenticate( config.username, config.password))
      {
        return request->requestAuthentication();
      }
      request->send(LittleFS, "/reboot.html", "text/html");
      saveConfiguration(configFile, config); 
      restartMCU=true;
    });  
}
void logout(){
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(401);
    });
}

void loggedout(){
  server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/logged-out.html","text/html");
    response->addHeader("ESPhostname", config.ESPhostname);
    request->send(response);
  });
}

void setuppage(){
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
    response->addHeader("output0Friendlyname", config.output0Friendlyname);
    response->addHeader("output0reverse", String(config.output0reverse));
    response->addHeader("output1Friendlyname", config.output1Friendlyname);
    response->addHeader("output1reverse", String(config.output1reverse));
    response->addHeader("output2Friendlyname", config.output2Friendlyname);
    response->addHeader("output2reverse", String(config.output2reverse));
    response->addHeader("output3Friendlyname", config.output3Friendlyname);
    response->addHeader("output3reverse", String(config.output3reverse));
    request->send(response);
    });
}

void configpage(){
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest * request) {
      if (!request->authenticate( config.username, config.password))
      {
        return request->requestAuthentication();
      }
      int params = request->params();
      config.output0reverse=false;
      config.output1reverse=false;
      config.output2reverse=false;
      config.output3reverse=false;
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
          if (!strcmp(p->name().c_str(), "password")){
            strlcpy(config.password,  p->value().c_str(), sizeof(config.password));
          }
          if (!strcmp(p->name().c_str(), "SSID")){
            strlcpy(config.SSID,  p->value().c_str(), sizeof(config.SSID));
          }
          if (!strcmp(p->name().c_str(), "APkey")){
            strlcpy(config.APkey,  p->value().c_str(), sizeof(config.APkey));
          }
          if (!strcmp(p->name().c_str(), "ESPhostname")){
            strlcpy(config.ESPhostname,  p->value().c_str(), sizeof(config.ESPhostname));
          }
          if (!strcmp(p->name().c_str(), "friendlyName")){
            strlcpy(config.friendlyName,  p->value().c_str(), sizeof(config.friendlyName));
          }
          if (!strcmp(p->name().c_str(), "mqtt1Server")){
            strlcpy(config.mqtt1Server,  p->value().c_str(), sizeof(config.mqtt1Server));
          }
          if (!strcmp(p->name().c_str(), "mqtt1User")){
            strlcpy(config.mqtt1User,  p->value().c_str(), sizeof(config.mqtt1User));
          }
          if (!strcmp(p->name().c_str(), "mqtt1Password")){
            strlcpy(config.mqtt1Password,  p->value().c_str(), sizeof(config.mqtt1Password));
          }
          if (!strcmp(p->name().c_str(), "mqtt1ClientId")){
            strlcpy(config.mqtt1ClientId,  p->value().c_str(), sizeof(config.mqtt1ClientId));
          }
          if (!strcmp(p->name().c_str(), "mqtt1Topic1")){
            strlcpy(config.mqtt1Topic1,  p->value().c_str(), sizeof(config.mqtt1Topic1));
          }
          if (!strcmp(p->name().c_str(), "mqtt1Topic2")){
            strlcpy(config.mqtt1Topic2,  p->value().c_str(), sizeof(config.mqtt1Topic2));
          }
          if (!strcmp(p->name().c_str(), "output0Friendlyname")){
            strlcpy(config.output0Friendlyname,  p->value().c_str(), sizeof(config.output0Friendlyname));
          }
          if (!strcmp(p->name().c_str(), "output1Friendlyname")){
            strlcpy(config.output1Friendlyname,  p->value().c_str(), sizeof(config.output1Friendlyname));
          }
          if (!strcmp(p->name().c_str(), "output2Friendlyname")){
            strlcpy(config.output2Friendlyname,  p->value().c_str(), sizeof(config.output2Friendlyname));
          }
          if (!strcmp(p->name().c_str(), "output3Friendlyname")){
            strlcpy(config.output3Friendlyname,  p->value().c_str(), sizeof(config.output3Friendlyname));
          }
          if (!strcmp(p->name().c_str(), "output0reverse")){
            if(!strcmp(p->value().c_str(),"on")){
              config.output0reverse=true;
            }
          }
          if (!strcmp(p->name().c_str(), "output1reverse")){
            if(!strcmp(p->value().c_str(),"on")){
              config.output1reverse=true;
            }
          }
          if (!strcmp(p->name().c_str(), "output2reverse")){
            if(!strcmp(p->value().c_str(),"on")){
              config.output2reverse=true;
            }
          }
          if (!strcmp(p->name().c_str(), "output3reverse")){
            if(!strcmp(p->value().c_str(),"on")){
              config.output3reverse=true;
            }
          }
      }
      
      request->redirect("/reboot");
      
  });
}

void resetpage(){
    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
      if (!request->authenticate(config.username, config.password))
        return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/reboot.html","text/html");
      response->addHeader("ESPhostname", config.ESPhostname);
      request->send(response);
      resetConfig();
      restartMCU=true;
    });
}
