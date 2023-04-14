#ifndef indexPage_h
#define indexPage_h

const char index_page[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP-Seismometer</title>
    <style>
      body {
        color: #fff;
        font-family: 'Open Sans', sans-serif;
        font-size: 15px;
        background: #060606;
        margin:0 auto;
        text-align:center;
      }

      .name {
        margin-top:25px;
        margin-bottom:25px;
        text-align: center;
        font-size: 30px;
      }

      .links {
        text-align: center;
        margin-top: 10px;
        margin-bottom: 10px;
        padding: 15px;
        border: 1px solid white;
        border-width: 2px;
        width: 290px;
        display: block;
        margin-left: auto;
        margin-right: auto;
        border-radius: 100px;
      }

      a {
        text-decoration: none;
        color: white;
        transition: color 0.5s; 
      }


      a:hover {
        color: #000;
        background: #fff;
      }
    </style>
  </head>
  <body>
    <div class="name">ESP-Seismometer</div>
    <a href="/log" target="blank" class="links">Dashboard & Log</a>
    <a href="/config" class="links">Wi-Fi Config</a>
    <a href="/update" class="links">Update OTA</a>
    <a href="/reboot" class="links">Restart ESP</a>
    <small>by NaufalAzkia</small>
</body></html>)rawliteral";

#endif
