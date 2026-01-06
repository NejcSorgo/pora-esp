#ifndef ZIGBEE_MODE_ROUTER
#error "Zigbee Router mode is not selected in Tools -> Zigbee Mode"
#endif
#include "Zigbee.h"

#define PLUG_PIN 8 
#define BUTTON_PIN 9 
ZigbeeSwitch zbPlug = ZigbeeSwitch(17); 

void setup() {
  Serial.begin(115200);
  
  pinMode(PLUG_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  zbPlug.setManufacturerAndModel("Espressif", "ESP32C6-SmartPlug");
  zbPlug.onChange(funcPlugChange);
  Zigbee.addEndpoint(&zbPlug);
  Serial.println("Starting Zigbee Router...");

  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    while (1);
  }
  Serial.println("Router started. Waiting to join network...");
}
void loop() {  

  if (digitalRead(BUTTON_PIN) == LOW) {
    
    delay(100); 
    if(digitalRead(BUTTON_PIN) == LOW) {
        bool currentState = digitalRead(PLUG_PIN);
        funcPlugChange(!currentState);
        zbPlug.report(); 
        while(digitalRead(BUTTON_PIN) == LOW); /
    }
  }
}

void funcPlugChange(bool state) {
  Serial.printf("Plug toggled: %s\n", state ? "ON" : "OFF");
  digitalWrite(PLUG_PIN, state ? HIGH : LOW);
}