#ifndef ZIGBEE_MODE_COORDINATOR
#error "Zigbee Coordinator mode is not selected in Tools -> Zigbee Mode"
#endif

#include "Zigbee.h"

#define BUTTON_PIN 9  // Boot button on most ESP32-C6 boards

/*
 * ZigbeeSwitch in Client mode (Controller).
 * This device sends commands, it doesn't receive them (for control purposes).
 */
ZigbeeSwitch zbRemote = ZigbeeSwitch(1);

// Variable to store the address of the light we want to control
uint16_t lightAddress = 0xFFFF; // 0xFFFF means unknown
uint8_t  lightEndpoint = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // 1. Setup the Remote (Client)
  zbRemote.setManufacturerAndModel("Espressif", "ESP32C6-Remote");
  Zigbee.addEndpoint(&zbRemote);

  Serial.println("Starting Zigbee Coordinator...");

  // 2. Start the network
  // true = clear configuration (New Network)
  // false = restore configuration
  if (!Zigbee.begin(false)) { 
    Serial.println("Failed to start Zigbee!");
    while (1);
  }

  // 3. Set the Tx Power (optional, max power)
  Zigbee.setRadioConfig(ZIGBEE_RADIO_NATIVE, 20); 

  Serial.println("Coordinator started!");
  Serial.println("Press BOOT button to Toggle lights.");
}

void loop() {
  // Check if Boot button is pressed
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); // Debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("Button Pressed!");
      toggleBoundLights();
      
      // Wait until button is released
      while (digitalRead(BUTTON_PIN) == LOW); 
    }
  }
}

/*
 * Logic to find and toggle lights
 */
void toggleBoundLights() {
  // If we don't know the light's address yet, try to find it
  if (lightAddress == 0xFFFF) {
    Serial.println("Scanning for On/Off Lights...");
    
    // Scan the network for devices with the "On/Off" capability
    // This finds the first available light in the network list
    zigbee_endpoint_info_t info = Zigbee.findEndpoint(ESP_ZB_HA_ON_OFF_LIGHT_DEVICE_ID);
    
    if (info.short_addr != 0xFFFF) {
      lightAddress = info.short_addr;
      lightEndpoint = info.endpoint;
      Serial.printf("Found Light at Addr: 0x%04X, Endpoint: %d\n", lightAddress, lightEndpoint);
      
      // Bind this remote to that light so we can talk to it
      zbRemote.bind(lightAddress, lightEndpoint);
    } else {
      Serial.println("No lights found yet. Make sure the Light is powered on.");
      return;
    }
  }

  // Send the Toggle command
  Serial.println("Sending TOGGLE command...");
  zbRemote.toggle(lightAddress, lightEndpoint);
}