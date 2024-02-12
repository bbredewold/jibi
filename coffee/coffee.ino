#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>
#include <ESP8266WebServer.h>
#include "Config.h"

// Pin Definitions
#define PIN_TX 12
#define PIN_RX 13

// Use SoftwareSerial for communication with Jura
SoftwareSerial juraSerial(PIN_RX, PIN_TX);

// Use the Webserver to expose the API
ESP8266WebServer server(80);

void connectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println(String("Connected to ") + WIFI_SSID + " with: " + WiFi.localIP().toString());
}

void setupWebServer() {
  server.on("/", []() {
    server.send(200, "text/plain", "Hello, lover of the golden brown!");
  });

  server.begin();
  MDNS.begin(MDNS_NAME);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(9600);
  juraSerial.begin(9600);

  connectToWifi();
  setupWebServer();
}

void loop() {
  server.handleClient();
  MDNS.update();
}