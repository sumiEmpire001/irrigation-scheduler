#include <Arduino.h>
#include <IrrigationScheduler.h>
// Potentiometer simulates soil moisture; LED indicates recommendation ONLY.
// Tank switch to ground means water available. Disconnected means empty.
#if defined(ARDUINO_ARCH_ESP32)
const int MOISTURE_POT=34, TANK_SWITCH=4, PUMP_LED=18;
const double ADC_MAX=4095.0;
#else
const int MOISTURE_POT=A0, TANK_SWITCH=7, PUMP_LED=8;
const double ADC_MAX=1023.0;
#endif
irrigation::Controller controller;
void setup() {
  Serial.begin(115200);
  pinMode(TANK_SWITCH,INPUT_PULLUP); pinMode(PUMP_LED,OUTPUT);
  digitalWrite(PUMP_LED,LOW);
}
void loop() {
  double moisture=analogRead(MOISTURE_POT)*100.0/ADC_MAX;
  bool tank_available=digitalRead(TANK_SWITCH)==LOW;
  // Demo timer is not safe across millis() rollover.
  irrigation::Report r=controller.ingest(millis()/1000.0,moisture,tank_available);
  digitalWrite(PUMP_LED,r.valid && r.pump_on?HIGH:LOW);
  Serial.print("moisture_pct="); Serial.print(moisture);
  Serial.print(" tank_available="); Serial.print(tank_available);
  Serial.print(" valid="); Serial.print(r.valid);
  Serial.print(" pump_LED="); Serial.println(r.pump_on);
  delay(1000);
}
