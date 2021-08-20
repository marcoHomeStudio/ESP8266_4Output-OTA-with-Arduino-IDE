<h1><span style="text-decoration: underline;">ESP8266_OTA-with-Arduino-IDE</span></h1>
<h2>Arduino IDE DEV platform with OTA based on ESP8266-01</h2>
<p><img src="./dashboard.png" alt="" /></p>
<p><span style="font-weight: 400;">Based on the popular esp8266-01 module ad arduino IDE,<br /></span><span style="font-weight: 400;">The idea is to make a programmable platform using the 4 gpio's as digital output/Input.<br /></span><span style="font-weight: 400;">It can be easily connected to any module like 4 relay module boards or anything needed to be driven by digital output.</span></p>
<p><span style="font-weight: 400;">Async web server and async web socket have been implemented as device dashboard and AsyncElegantOTA to upload the firmware and the FS data folder.<br /></span><span style="font-weight: 400;">A config.ini file is stored in the data folder for the device parameter.<br /></span><span style="font-weight: 400;">The web server html and css and js file are stored in the Data directory.</span></p>
<p><span style="font-weight: 400;">Just copy the ESP8266_OTA.ino file and the data folder to your arduino sketch folder.<br /></span><span style="font-weight: 400;">Modify your wifi credential in the sketch<br /></span><span style="font-weight: 400;">In the config.inifile modify<br /></span><span style="color: #000080;"><em><strong>username:admin,<br /></strong></em></span><span style="font-weight: 400;"><span style="color: #000080;"><em><strong>password:12345</strong></em></span><br /></span><span style="font-weight: 400;">To change the web server credential</span></p>
<p><span style="font-weight: 400;">The following libraries must be installed:</span></p>
<ul>
<li><a href="https://github.com/me-no-dev/ESPAsyncTCP">ESPAsyncTCP</a></li>
<li>
<div>
<div><a href="https://github.com/me-no-dev/ESPAsyncWebServer">ESPAsyncWebServer</a></div>
</div>
</li>
<li><a href="https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases">LittleFS </a></li>
<li><a href="https://arduinojson.org/">ArduinoJSON </a></li>
<li><a href="https://github.com/ayushsharma82/AsyncElegantOTA">AsyncElegantOTA</a></li>
</ul>
