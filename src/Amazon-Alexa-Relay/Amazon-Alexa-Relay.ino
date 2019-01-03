#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include "fauxmoESP.h"
#include "html.h"

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE     115200

#define PIN_RELAY          D1

#define ID_RELAY           "Socket"

boolean relayOn = false;

void setup() {

  // Init serial port and clean garbage
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();
  Serial.println();

  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);


  fauxmo.createServer(true);
  fauxmo.setPort(80);

  fauxmo.enable(true);

  fauxmo.addDevice(ID_RELAY);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

    if (strcmp(device_name, ID_RELAY) == 0) {
      digitalWrite(PIN_RELAY, state ? HIGH : LOW);
      relayOn = state ? HIGH : LOW;
    }

  });
}

void loop() {
  fauxmo.handle();

  static unsigned long last = millis();
  if (millis() - last > 5000) {
    last = millis();
    Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
  }

  if (digitalRead(PIN_RELAY) == HIGH) {
    relayOn = true;
  } else {
    relayOn = false;
  }

  fauxmo.setState(ID_RELAY, relayOn, 255);
}
