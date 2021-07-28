<h1><span style="text-decoration: underline;"># ESP8266_OTA</span></h1>
<h2>DEV platform with OTA based on ESP8266-01</h2>
<p><img src="/dashboard.png" alt="" /></p>
<p>Based on the popular esp8266-01 module,<br />The idea is to make a programmable platform using the 4 gpio's as digital output/Input.<br />It can be easily connected to any module like 4 relay module board or anything needed to be driven by digital output.<br />Async web server and async web socket have been implemented as device dashboard and AsyncElegantOTA to upload the firmware and the FS data folder.<br /> The web server html and css and js file are stored in the Data directory.</p>
<p>The folowing libraries must be installed:</p>
<ul>
<li><a href="https://github.com/me-no-dev/ESPAsyncTCP">ESPAsyncTCP</a></li>
<li>
<div>
<div><a href="https://github.com/me-no-dev/ESPAsyncWebServer">ESPAsyncWebServer</a></div>
</div>
</li>
<li><a href="https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases">LittleFS </a></li>
<li><a href="https://www.arduino.cc/reference/en/libraries/arduino_json/">Arduino_JSON </a></li>
<li><a href="https://github.com/ayushsharma82/AsyncElegantOTA">AsyncElegantOTA</a></li>
</ul>