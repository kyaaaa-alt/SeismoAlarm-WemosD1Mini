#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266HTTPClient.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "LittleFS.h"
#include <ESPDash.h>

#define minval -0.03
#define maxval 0.09
#define minwarn -0.02
#define maxwarn 0.08

const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";
const char* PARAM_INPUT_5 = "staticip";
const char* PARAM_INPUT_6 = "usepass";

String ssid;
String pass;
String ip;
String gateway;
String staticip;
String usepass;

const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";
const char* staticipPath = "/staticip.txt";
const char* usepassPath = "/usepass.txt";

IPAddress localIP;
IPAddress localGateway;
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(1, 1, 1, 1);

const int buzzPin=2; //D4
const int timeOn=150;
const int timeOff=25;
const int wifi_led=14; //D5
const int warn_led=12; //D6
const int alarm_led=13; //D7

boolean restart = false;

AsyncWebServer server(80);
ESPDash dashboard(&server);
Card temperature(&dashboard, TEMPERATURE_CARD, "Temperature", "°C");
Card axisx(&dashboard, GENERIC_CARD, "X Axis");
Card axisy(&dashboard, GENERIC_CARD, "Y Axis");
Card axisz(&dashboard, GENERIC_CARD, "Z Axis");

const char* host = "discord.com";
const int httpsPort = 443;
const char fingerprint[] PROGMEM = "A3 EA 27 1A 3D E8 8C 05 5E 1C C8 1D 59 0E D2 F2 A1 76 4D 2E";

unsigned long wifiPrevMillis = 0;
unsigned long wifiInterval = 30000;

Adafruit_MPU6050 mpu;

#include "indexPage.h"
#include "configPage.h"
#include "initWiFi.h"
#include "mpu6050.h"
#include "alarm.h"
#include "fsys.h"

void setup() {
  pinMode(wifi_led, OUTPUT);
  pinMode(alarm_led, OUTPUT);
  pinMode(warn_led, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  Serial.begin(115200);
  while (!Serial)
    delay(10);
  digitalWrite(buzzPin, HIGH);
  digitalWrite(alarm_led, LOW);
  digitalWrite(warn_led, LOW);
  digitalWrite(wifi_led, LOW);
  delay(335);
  
  initFS();
  
  // Load values saved in LittleFS
  ssid = readFile(LittleFS, ssidPath);
  pass = readFile(LittleFS, passPath);
  ip = readFile(LittleFS, ipPath);
  gateway = readFile (LittleFS, gatewayPath);
  staticip = readFile (LittleFS, staticipPath);
  usepass = readFile (LittleFS, usepassPath);
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);
  Serial.println(gateway);
  Serial.println(staticip);
  Serial.println(usepass);

  if(initWiFi()) {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_page);
    });
    
    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
      restart = true;
      request->send(200, "text/plain", "Rebooting...");
    });
    
    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", config_page);
    });
    
    server.serveStatic("/", LittleFS, "/");
    
    server.on("/do_config", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(LittleFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(LittleFS, passPath, pass.c_str());
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_3) {
            ip = p->value().c_str();
            Serial.print("IP Address set to: ");
            Serial.println(ip);
            // Write file to save value
            writeFile(LittleFS, ipPath, ip.c_str());
          }
          // HTTP POST gateway value
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
            Serial.print("Gateway set to: ");
            Serial.println(gateway);
            // Write file to save value
            writeFile(LittleFS, gatewayPath, gateway.c_str());
          }
          // HTTP POST staticip value
          if (p->name() == PARAM_INPUT_5) {
            staticip = p->value().c_str();
            Serial.print("staticip set to: ");
            Serial.println(staticip);
            // Write file to save value
            writeFile(LittleFS, staticipPath, staticip.c_str());
          }
          // HTTP POST usepass value
          if (p->name() == PARAM_INPUT_6) {
            usepass = p->value().c_str();
            Serial.print("usepass set to: ");
            Serial.println(usepass);
            // Write file to save value
            writeFile(LittleFS, usepassPath, usepass.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      restart = true;
      request->send(200, "text/plain", "Selesai, perangkat akan melakukan restart dan tunggu selama kurang lebih 1 menit");
    });
    server.begin();
  }
  else {
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("ESP-Seismometer", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_page);
    });

    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
      restart = true;
      request->send(200, "text/plain", "Rebooting...");
    });
    
    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", config_page);
    });
    
    server.serveStatic("/", LittleFS, "/");
    
    server.on("/do_config", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(LittleFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(LittleFS, passPath, pass.c_str());
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_3) {
            ip = p->value().c_str();
            Serial.print("IP Address set to: ");
            Serial.println(ip);
            // Write file to save value
            writeFile(LittleFS, ipPath, ip.c_str());
          }
          // HTTP POST gateway value
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
            Serial.print("Gateway set to: ");
            Serial.println(gateway);
            // Write file to save value
            writeFile(LittleFS, gatewayPath, gateway.c_str());
          }
          // HTTP POST staticip value
          if (p->name() == PARAM_INPUT_5) {
            staticip = p->value().c_str();
            Serial.print("staticip set to: ");
            Serial.println(staticip);
            // Write file to save value
            writeFile(LittleFS, staticipPath, staticip.c_str());
          }
          // HTTP POST usepass value
          if (p->name() == PARAM_INPUT_6) {
            usepass = p->value().c_str();
            Serial.print("usepass set to: ");
            Serial.println(usepass);
            // Write file to save value
            writeFile(LittleFS, usepassPath, usepass.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      restart = true;
      request->send(200, "text/plain", "Selesai, perangkat akan melakukan restart dan tunggu selama kurang lebih 1 menit");
    });
    AsyncElegantOTA.begin(&server);
    server.begin();
  }
  Serial.println("WebServer Running...");
  mpu6050();
}

void loop() {
  if (restart){
    delay(2000);
    ESP.restart();
  }
  AsyncElegantOTA.loop();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  if(minval >= g.gyro.x || minval >= g.gyro.y || minval >= g.gyro.z || maxval <= g.gyro.x || maxval <= g.gyro.y || maxval <= g.gyro.z){
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("ALARM");
      digitalWrite(buzzPin, LOW);
      digitalWrite(alarm_led, HIGH);
      Serial.println("Trying send notification");
      
      HTTPClient http;
      WiFiClientSecure client;
      client.setFingerprint(fingerprint);
      client.connect(host, httpsPort);
      String endPoint = "/api/webhooks/10463612223323296/_S8FX2c2qweqweadA_D_0h9AnoAVWqq119exZAgKvhb2HbLnYx8X27r4og";
      String discordJSON = "{\"username\":\"SeismoBot\",\"embeds\":[{\"title\":\"Earthquake Early Warning Device Triggered\",\"color\":10181046,\"thumbnail\":{\"url\":\"https://cdn-icons-png.flaticon.com/128/1809/1809634.png\"},\"fields\":[{\"name\":\"Messages\",\"value\":\"Sensor Seismometer Mendeteksi Getaran!\"},{\"name\":\"To Do\",\"value\":\"Segera berlindung ke area selter terdekat atau ke area terbuka!\"}]}],\"content\":\"<@&1096361609133305986> \",\"allowed_mentions\":{\"roles\":[\"1096361609133305986\"]}}";
               
      client.print(String("POST ") + endPoint + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + discordJSON.length() + "\r\n" +
               "\r\n" + // This is the extra CR+LF pair to signify the start of a body
               discordJSON + "\n");
            
      client.print(String("GET ") + endPoint + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +               
               "Connection: close\r\n\r\n");
    }
    digitalWrite(wifi_led, LOW);
    delay(25);
    alarm();
    digitalWrite(alarm_led, LOW);
    digitalWrite(warn_led, LOW);
    digitalWrite(buzzPin, HIGH);
  } 

  if(minwarn >= g.gyro.x || minwarn >= g.gyro.y || minwarn >= g.gyro.z || maxwarn <= g.gyro.x || maxwarn <= g.gyro.y || maxwarn <= g.gyro.z) {
    digitalWrite(wifi_led, LOW);
    digitalWrite(warn_led, LOW);
    delay(10);
    Serial.println("WARNING");
    digitalWrite(buzzPin, LOW);
    digitalWrite(warn_led, HIGH);
    digitalWrite(wifi_led, LOW);
    delay(10);
    digitalWrite(buzzPin, HIGH);
    digitalWrite(wifi_led, LOW);
    digitalWrite(warn_led, LOW);
    delay(10);
    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(wifi_led, HIGH);
    }
    if (WiFi.status() != WL_CONNECTED) {
      digitalWrite(wifi_led, HIGH);
      digitalWrite(warn_led, HIGH);
    }
  }

  unsigned long wifiCurrMillis = millis();
  
  if ((WiFi.status() != WL_CONNECTED) && (wifiCurrMillis - wifiPrevMillis >=wifiInterval)) {
    if(ssid != ""){
      Serial.println("Reconnecting to WiFi...");
      WiFi.disconnect();
      if (usepass == "y") {
        WiFi.begin(ssid.c_str(), pass.c_str());
      } else {
        WiFi.begin(ssid.c_str());
      }
    }
    digitalWrite(wifi_led, HIGH);
    digitalWrite(warn_led, HIGH);
    wifiPrevMillis = wifiCurrMillis;
  } else if ((WiFi.status() == WL_CONNECTED) && (wifiCurrMillis - wifiPrevMillis >=wifiInterval)) {
    digitalWrite(wifi_led, HIGH);
    digitalWrite(warn_led, LOW);
    wifiPrevMillis = wifiCurrMillis;
  }
  
  temperature.update((float)temp.temperature);
  axisx.update((float)g.gyro.x);
  axisy.update((float)g.gyro.y);
  axisz.update((float)g.gyro.z);

  dashboard.sendUpdates();
  
  Serial.println("");
}
