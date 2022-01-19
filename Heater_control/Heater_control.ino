/*
 * 
 *    ESP32         CC1101
 *    -----         ------
 *    4   <-------> GDO2
 *    18  <-------> SCK
 *    3v3 <-------> VCC
 *    23  <-------> MOSI
 *    19  <-------> MISO
 *    5   <-------> CSn
 *    GND <-------> GND
 * 
 */
#include <Arduino.h>
#include "ArduinoNvs.h"
#include <LiquidCrystal.h>
#include "DieselHeaterRF.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>

#define HEATER_POLL_INTERVAL  4000

DieselHeaterRF heater;
heater_state_t state;
WiFiClientSecure wifi;
MQTTClient client(1024);

unsigned long lastMillis = 0;
int uptemp = 22;
int lowtemp = 20;
bool res;
const char* ssid = "Workshop";
const char* wifipassword = "naujasnaujas";
//Screen setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
uint32_t heaterAddr = 0; // Heater address is a 32 bit unsigned int. Use the findAddress() to get your heater's address.

//MQTT setup
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {

  NVS.begin();

  WiFi.begin(ssid, wifipassword);
  while( WiFi.status() != WL_CONNECTED ) {
      Serial.print(".");
      delay(1000);
  }
  Serial.print("Connected to ");
  Serial.println(ssid);

  client.begin("192.168.0.108", 8883, wifi);
  client.onMessage(messageReceived);
  
  Serial.begin(115200);

  heater.begin();
  
  uint32_t heaterAddr = NVS.getInt("Addr");
   
  if(heaterAddr = 0){
  Serial.println("Started pairing, press and hold the pairing button on the heater's LCD panel...");
  heaterAddr = heater.findAddress(60000UL);
  
  if (heaterAddr) {
    Serial.print("Got address: ");
    Serial.println(heaterAddr, HEX);
    heater.setAddress(heaterAddr);

    res = NVS.setInt("Addr", heaterAddr);// Store the address somewhere, eg. NVS    

   
  } else {
    Serial.println("Failed to find a heater");   
    while(1) {}
  }
  
}
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    if (!client.connected()) {
      Serial.print("lastError: ");
      Serial.println(client.lastError());
    }
    lastMillis = millis();
    client.publish("/hello", "world");
  }

  heater.sendCommand(HEATER_CMD_WAKEUP);
  heater.getState(&state);
  Serial.print("State: %d");
  Serial.print(state.state);
  Serial.print("Power: %d");
  Serial.print(state.power);
  Serial.print("Voltage: %d");
  Serial.print(state.voltage);
  Serial.print("ATemperature: %d");
  Serial.print(state.ambientTemp);
  Serial.print("CTemperature: %d");
  Serial.print(state.caseTemp);
  Serial.print("Setpoint: %d");
  Serial.print(state.setpoint);
  Serial.print("AutoMode: %d");
  Serial.print(state.autoMode);
  Serial.print("Pump Freq: %d");
  Serial.print(state.pumpFreq);
  Serial.print("RSSI: %d");
  Serial.print(state.rssi);

  
  Serial.print(state.ambientTemp);
  lcd.print("Temperature: %d");
  lcd.print(state.ambientTemp);
    
  if ((state.ambientTemp < lowtemp) && (state.state = HEATER_STATE_OFF)) {
   heater.sendCommand(HEATER_CMD_POWER);
   Serial.print("Turn on!");
  } 
  
  if ((state.ambientTemp > uptemp) && (state.state != HEATER_STATE_OFF)) {
   heater.sendCommand(HEATER_CMD_POWER);  
   lcd.print("Turn off!");
  }
  
  delay(HEATER_POLL_INTERVAL);

}
