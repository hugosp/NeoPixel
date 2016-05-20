#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <hash.h>
#include "conf.h"

#define PIN D5
#define NUMPIXELS 8

int pixels = 0;

ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendTXT(num, "Connected to ESP");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);

      if(payload[0] == '$') {
        int nummer = atoi((const char *)&payload[1]);
        if (nummer<pixels) {
          for(int i=nummer;i<(NUMPIXELS);i++){
            strip.setPixelColor(i, 0);
          }
          strip.show();   
        }
        pixels = nummer;
      }
      
      if(payload[0] == '#') {
        
        uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);

        for(int i=0;i<pixels;i++){
          strip.setPixelColor(i, strip.Color(((rgb >> 16) & 0xFF),((rgb >> 8) & 0xFF),((rgb >> 0) & 0xFF)));
        }
        strip.show();
      }
      break;
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFiMulti.addAP(SSID, PASSWORD);
  while(WiFiMulti.run() != WL_CONNECTED) {
      delay(100);
  }

  server.on("/", []() {
      server.send(200, "text/html", "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);};function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);  var g = parseInt(document.getElementById('g').value).toString(16);  var b = parseInt(document.getElementById('b').value).toString(16);  if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }   if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;    console.log('RGB: ' + rgb); connection.send(rgb); }</script></head><body>LED Control:<br/><br/>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" onmousemove=\"sendRGB();\" onchange=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" onmousemove=\"sendRGB();\" onchange=\"sendRGB();\" /><br/>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" onmousemove=\"sendRGB();\" onchange=\"sendRGB();\" /><br/></body></html>");
  });


  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  server.begin();
  strip.begin();
  strip.show();
}

void loop() {
  webSocket.loop();
  server.handleClient();
}
