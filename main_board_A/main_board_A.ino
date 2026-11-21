#include <Wire.h>
#include "Multizone_ToF.h"
#include "Single_Zone_ToF.h"

// Direct ESP32 I2C test (no TCA9548A mux).
// Note: Both VL53L5CX and VL53L1X default to I2C address 0x29.
// Test one sensor at a time unless one has its I2C address reassigned.

MultizoneToF multizoneSensor;
SingleZoneToF singleZoneSensor("Single-Zone");

bool multizoneReady = false;
bool singleZoneReady = false;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000)
    ;  // Wait up to 3s for Serial monitor

  Wire.begin(21, 22);     // ESP32 default I2C pins (SDA = 21, SCL = 22)
  Wire.setClock(400000);  // 400 kHz Fast-Mode for VL53L5CX

  Serial.println("Initializing direct ToF sensors...");

  multizoneReady = multizoneSensor.init();
  Serial.println(multizoneReady ? "[OK] VL53L5CX (Multizone) ready." : "[SKIP] VL53L5CX (Multizone) not detected.");

  singleZoneReady = singleZoneSensor.init();
  Serial.println(singleZoneReady ? "[OK] VL53L1X (Single-Zone) ready." : "[SKIP] VL53L1X (Single-Zone) not detected.");
}

void loop() {
  // Read and print closest valid target from the 8x8 multizone sensor
  if (multizoneReady) {
    MultizoneTarget target;
    if (multizoneSensor.read(target))
      Serial.printf("[Multizone] %d mm (Row %d, Col %d)\n", target.minDistance_mm, target.row, target.col);
  }

  // Read and print distance from the single-zone sensor
  if (singleZoneReady) {
    uint16_t distance = 0;
    if (singleZoneSensor.read(distance))
      Serial.printf("[%s] %d mm\n", singleZoneSensor.getLabel(), distance);
  }

  delay(30);  // Yield to prevent triggering ESP32 Task Watchdog Timer
}