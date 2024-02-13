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

String sendAndReceiveData(String outgoingData) {
  String incomingData;
  int bitIndex = 0;
  int timeoutCounter = 0;

  while (juraSerial.available()) {
    juraSerial.read();
  }

  outgoingData += "\r\n";
  for (int i = 0; i < outgoingData.length(); i++) {
    for (int bitPos = 0; bitPos < 8; bitPos += 2) {
      char rawByte = 255;
      bitWrite(rawByte, 2, bitRead(outgoingData.charAt(i), bitPos));
      bitWrite(rawByte, 5, bitRead(outgoingData.charAt(i), bitPos + 1));
      juraSerial.write(rawByte);
    }
    delay(8);
  }

  int bitPos = 0;
  char receivedByte;
  while (!incomingData.endsWith("\r\n")) {
    if (juraSerial.available()) {
      byte rawByte = juraSerial.read();
      bitWrite(receivedByte, bitPos, bitRead(rawByte, 2));
      bitWrite(receivedByte, bitPos + 1, bitRead(rawByte, 5));
      if ((bitPos += 2) >= 8) {
        bitPos = 0;
        incomingData += receivedByte;
      }
    } else {
      delay(10);
    }
    
    if (timeoutCounter++ > 500) {
      return "";
    }
  }

  return incomingData.substring(0, incomingData.length() - 2);
}


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

  server.on("/make-coffee", []() {
    String response;

    response = sendAndReceiveData("FA:01"); // Make normal coffee
    server.send(200, "application/json", "{\"status\": \"ok\", \"message\": \"One coffee on the way!\", \"response\": \"" + response + "\"}");
  });

  server.begin();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(9600);
  juraSerial.begin(9600);

  connectToWifi();
  setupWebServer();

  MDNS.begin(MDNS_NAME);
}

void loop() {
  server.handleClient();
  MDNS.update();
}