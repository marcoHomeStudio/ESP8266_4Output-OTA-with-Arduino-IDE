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
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/reboot.html","text/html");
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
    request->send(response);
    });
}

void configpage(){
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
