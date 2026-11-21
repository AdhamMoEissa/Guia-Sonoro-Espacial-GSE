#include "Spatial_Nav.h"
#include <Wire.h>

// I2C Multiplexer Channel Assignments
#define MUX_CH_FRONT_MZ 0
#define MUX_CH_FL_SZ 1
#define MUX_CH_FR_SZ 2
#define MUX_CH_REAR_SZ 3

// File-scoped sensor instances (hidden from main.ino)
static MultizoneToF frontSensor(MUX_CH_FRONT_MZ);
static SingleZoneToF leftSensor(MUX_CH_FL_SZ, "Front-Left");
static SingleZoneToF rightSensor(MUX_CH_FR_SZ, "Front-Right");
static SingleZoneToF rearSensor(MUX_CH_REAR_SZ, "Rear");

void initSpatialSensors() {
  Wire.begin(21, 22);     // ESP32 default I2C pins (SDA = 21, SCL = 22)
  Wire.setClock(400000);  // 400 kHz Fast-Mode required for VL53L5CX throughput

  Serial.println("=======================================");
  Serial.println(" Wearable Spatial Array Initialization ");
  Serial.println("=======================================");

  Serial.println("Flashing firmware to VL53L5CX (Ch 0)...");
  if (!frontSensor.begin()) {
    Serial.println("[FAIL] Front Multizone VL53L5CX not responding!");
  } else {
    Serial.println("[OK] Front Multizone VL53L5CX ready.");
  }

  // Initialize each single-zone sensor
  SingleZoneToF* singleZoneList[] = { &leftSensor, &rightSensor, &rearSensor };
  for (SingleZoneToF* s : singleZoneList) {
    if (!s->begin()) {
      Serial.printf("[FAIL] %s VL53L1X failed initialization!\n", s->getLabel());
    } else {
      Serial.printf("[OK] %s VL53L1X ready.\n", s->getLabel());
    }
  }

  Serial.println("\nAll optical distance buses initialized.\n");
}

SpatialData readSpatialSensors() {
  SpatialData data = {};  // Zero-initialize all fields and validity flags

  data.frontLeftValid = leftSensor.read(data.frontLeft_mm);
  data.frontRightValid = rightSensor.read(data.frontRight_mm);
  data.rearValid = rearSensor.read(data.rear_mm);
  frontSensor.read(data.front);

  return data;
}

void printSpatialDebug(const SpatialData& data) {
  if (data.frontLeftValid)
    Serial.printf("[FL]: %4d mm | ", data.frontLeft_mm);
  
  if (data.frontRightValid)
    Serial.printf("[FR]: %4d mm | ", data.frontRight_mm);
  
  if (data.rearValid)
    Serial.printf("[REAR]: %4d mm | ", data.rear_mm);
  
  if (data.front.detected)
    Serial.printf("[FRONT MATRIX]: %4d mm at (Row %d, Col %d)", data.front.minDistance_mm, data.front.row, data.front.col);

  Serial.println();
}