#ifndef ZIGBEE_MODE_ROUTER
#error "Zigbee Router mode is not selected in Tools -> Zigbee Mode"
#endif

#include "Zigbee.h"

// Pin for the Relay/LED
#define PLUG_PIN 8 
#define BUTTON_PIN 9 // Boot button on C6/H2 to toggle manually

/*
 * ZigbeeSwitch represents a standard "On/Off Plug-in Unit"
 * Endpoint: 10
 */
ZigbeeSwitch zbPlug = ZigbeeSwitch(10); 

void setup() {
  Serial.begin(115200);
  
  pinMode(PLUG_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Set Manufacturer info
  zbPlug.setManufacturerAndModel("Espressif", "ESP32C6-SmartPlug");
  
  // Register callback for network commands
  zbPlug.onChange(funcPlugChange);

  Zigbee.addEndpoint(&zbPlug);

  Serial.println("Starting Zigbee Router...");
  
  // Start Zigbee
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    while (1);
  }
  Serial.println("Router started. Waiting to join network...");
}

void loop() {  
  // Simple logic to toggle plug via physical button
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Debounce
    delay(100); 
    if(digitalRead(BUTTON_PIN) == LOW) {
        bool currentState = digitalRead(PLUG_PIN);
        // Toggle local state
        funcPlugChange(!currentState);
        // Report new state to the Hub/Coordinator
        zbPlug.report(); 
        while(digitalRead(BUTTON_PIN) == LOW); // Wait for release
    }
  }
}

void funcPlugChange(bool state) {
  Serial.printf("Plug toggled: %s\n", state ? "ON" : "OFF");
  digitalWrite(PLUG_PIN, state ? HIGH : LOW);
}