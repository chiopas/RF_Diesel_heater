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
#include "DieselHeaterRF.h"


#define HEATER_POLL_INTERVAL  4000

DieselHeaterRF heater;
heater_state_t state;


uint32_t heaterAddr; // Heater address is a 32 bit unsigned int. Use the findAddress() to get your heater's address.

void setup() {
  
  Serial.begin(115200);

  heater.begin();
   
  Serial.println("Started pairing, press and hold the pairing button on the heater's LCD panel...");
  
  heaterAddr = heater.findAddress(60000UL);
  
  if (heaterAddr) {
    Serial.print("Got address: ");
    Serial.println(heaterAddr, HEX);
    heater.setAddress(heaterAddr);
  }
 else {
    Serial.println("Failed to find a heater");    
    while(1) {}
  }
  
}
void loop() {

  heater.sendCommand(HEATER_CMD_WAKEUP);
  
  heater.getState(&state);
  Serial.println("State:");
  Serial.println(state.state);
    delay(1000);
  Serial.println("Ambient Temperature:");
  Serial.println(state.ambientTemp);
    delay(1000);
  Serial.println("Case Temperature:");
  Serial.println(state.caseTemp);
    delay(1000);
  Serial.println("Setpoint Temperature:");
  Serial.println(state.setpoint);
    delay(1000);

    
  if ((state.ambientTemp <= state.setpoint - 2) && (state.state == HEATER_STATE_OFF)) {
   heater.sendCommand(HEATER_CMD_POWER);
   Serial.print("Turn on!");
  } 
  
  if ((state.ambientTemp >= state.setpoint + 1) && (state.state == HEATER_STATE_RUNNING)) {
   heater.sendCommand(HEATER_CMD_POWER);  
   Serial.print("Turn off!");
  }
  
  delay(HEATER_POLL_INTERVAL);

}
