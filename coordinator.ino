#ifndef ZIGBEE_MODE_COORDINATOR
#error "Zigbee Coordinator mode is not selected in Tools -> Zigbee Mode"
#endif

#include "Zigbee.h"

#define BUTTON_PIN 9 

ZigbeeSwitch zbRemote = ZigbeeSwitch(1);


uint16_t lightAddress = 0xFFFF; // 0xFFFF means unknown
uint8_t  lightEndpoint = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  zbRemote.setManufacturerAndModel("Espressif", "ESP32C6-Remote");
  Zigbee.addEndpoint(&zbRemote);

  Serial.println("Starting Zigbee Coordinator...");

  if (!Zigbee.begin(false)) { // v primeru ce ne spila 
    Serial.println("Failed to start Zigbee!");
    while (1);
  }

  Zigbee.setRadioConfig(ZIGBEE_RADIO_NATIVE, 20); 

  Serial.println("Coordinator started!");
  Serial.println("Press BOOT button to Toggle lights.");
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); 
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("Button Pressed!");
      toggleBoundLights();
      
      while (digitalRead(BUTTON_PIN) == LOW); 
    }
  }
}

void toggleBoundLights() {
  if (lightAddress == 0xFFFF) {
    Serial.println("Scanning for On/Off Lights...");
   
    zigbee_endpoint_info_t info = Zigbee.findEndpoint(ESP_ZB_HA_ON_OFF_LIGHT_DEVICE_ID);
    
    if (info.short_addr != 0xFFFF) {
      lightAddress = info.short_addr;
      lightEndpoint = info.endpoint;
      Serial.printf("Found Light at Addr: 0x%04X, Endpoint: %d\n", lightAddress, lightEndpoint);
      
      zbRemote.bind(lightAddress, lightEndpoint);
    } else {
      Serial.println("No lights found yet. Make sure the Light is powered on.");
      return;
    }
  }
  Serial.println("Sending TOGGLE command...");
  zbRemote.toggle(lightAddress, lightEndpoint);
}