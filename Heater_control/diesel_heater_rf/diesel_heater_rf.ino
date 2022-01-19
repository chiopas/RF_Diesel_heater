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

#include <DieselHeaterRF.h>
#include <LiquidCrystal.h>

#define HEATER_POLL_INTERVAL  4000

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
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
    Serial.print("State: %d");
    Serial.print(state.state);
  } else {
    lcd.print("Failed to get the state");
  }
  
  delay(HEATER_POLL_INTERVAL);

}
