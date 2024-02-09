#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>
#include <ESP8266WebServer.h>
#include "Config.h"

// PIN Definitions
#define GPIO_TX 12
#define GPIO_RX 13

// Use SoftwareSerial for communication with Jura
SoftwareSerial softserial(GPIO_RX, GPIO_TX);

// Use the Webserver to expose the API
ESP8266WebServer web(80);

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println(String("* Connected to ") + WIFI_SSID + " with: " + WiFi.localIP().toString());
}

void setupWebserver() {
  web.on("/", []() {
    web.send(200, "text/plain", "Hello, lover of the golden brown!");
  });

  web.begin();
  Serial.println("* HTTP server started");

  MDNS.begin(MDNS_NAME);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(9600);
  softserial.begin(9600);

  setupWifi();
  setupWebserver();
}

void loop() {
  web.handleClient();
  MDNS.update();
}