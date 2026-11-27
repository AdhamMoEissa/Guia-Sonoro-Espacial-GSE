#ifndef SPATIAL_NAV_H
#define SPATIAL_NAV_H

#include <Arduino.h>
#include "Multizone_ToF.h"
#include "Single_Zone_ToF.h"

// Unified snapshot of all optical sensors (ready for FreeRTOS queue transfer)
struct SpatialData {
  MultizoneTarget front;
  uint16_t frontLeft_mm;
  uint16_t frontRight_mm;
  uint16_t rear_mm;
  bool frontLeftValid;
  bool frontRightValid;
  bool rearValid;
};

// Initializes I2C and all spatial ToF sensors.
void initSpatialSensors();

// Polls all sensors and returns the latest spatial snapshot.
SpatialData readSpatialSensors();

// Prints valid sensor readings to Serial.
void printSpatialDebug(const SpatialData &data);

#endif // SPATIAL_NAV_H