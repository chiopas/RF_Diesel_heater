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

#include "DieselHeaterRF.h"

#define HEATER_POLL_INTERVAL  4000

uint32_t heaterAddr; // Heater address is a 32 bit unsigned int. Use the findAddress() to get your heater's address.

DieselHeaterRF heater;
heater_state_t state;

void setup() {

  Serial.begin(115200);

  heater.begin();

  Serial.println("Started pairing, press and hold the pairing button on the heater's LCD panel...");

  heaterAddr = heater.findAddress(60000UL);

  if (heaterAddr) {
    Serial.print("Got address: ");
    Serial.println(heaterAddr, HEX);
    heater.setAddress(heaterAddr);
    // Store the address somewhere, eg. NVS
  } else {
    Serial.println("Failed to find a heater");   
    while(1) {}
  }
  
}

void loop() {

  heater.sendCommand(HEATER_CMD_WAKEUP);

  if (heater.getState(&state)) {
//    Serial.print("State: %d", state.state);
    Serial.print("Power: %d", state.power);
    Serial.print("Voltage: %d", state.voltage);
    Serial.print("Ambient: %d", state.ambientTemp);
    Serial.print("Case: %d", state.caseTemp);
    Serial.print("Setpoint: %d", state.setpoint);
    Serial.print("PumpFreq: %d", state.pumpFreq);
    Serial.print("Auto: %d", state.autoMode);
    Serial.print("RSSI: %d", state.rssi);
  } else {
    Serial.println("Failed to get the state");
  }
  
  delay(HEATER_POLL_INTERVAL);

}
