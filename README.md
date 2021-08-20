# ESP8266_4Output-OTA-with-Arduino-IDE
## Arduino IDE DEV platform with OTA based on ESP8266-01
![Esp8266 dashboard](./Doc/dashboard.png)
Based on the popular esp8266-01 module ad arduino IDE,  
The idea is to make a programmable platform using the 4 gpio's as digital output/Input.  
It can be easily connected to any module like 4 relay module boards or anything needed to be driven by digital output.

Async web server and async web socket have been implemented as device dashboard and AsyncElegantOTA to upload the firmware and the FS data folder.  
A config.json file is stored in the data folder for the device parameter.  
The web server html and css and js file are stored in the Data directory.

## Installation
Just copy the **ESP8266_4output_OTA.ino** file and the **data** folder to your Arduino sketch folder. 
Upload the sketch from Arduino IDE 
Upload the ***data*** folder with the ***[ESP8266 Little FS data upload](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin)*** python script form the arduino IDE 

## Dependencies
The following libraries must be installed:

-   [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP)
-   [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
    
-   [LittleFS](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases)
-   [ArduinoJSON](https://arduinojson.org/)
-   [AsyncElegantOTA](https://github.com/ayushsharma82/AsyncElegantOTA)
- 
