#ifndef MULTIZONE_TOF_H
#define MULTIZONE_TOF_H

#include <Arduino.h>
#include <SparkFun_VL53L5CX_Library.h>
#include "TCA_Mux.h"

// Closest valid target found in the 8x8 multizone grid.
struct MultizoneTarget {
  bool detected;
  uint16_t minDistance_mm;
  uint8_t row;
  uint8_t col;
  uint8_t zoneIndex;
};

// Sentinel channel value for direct I2C connection without a multiplexer
#define NO_MUX_CHANNEL 255

// Wrapper for the VL53L5CX 8x8 multizone ToF sensor (direct or behind a TCA9548A mux).
class MultizoneToF {
public:
  MultizoneToF(uint8_t muxChannel = NO_MUX_CHANNEL);

  bool init();
  bool begin();
  bool read(MultizoneTarget &result);

private:
  uint8_t _muxChannel;
  SparkFun_VL53L5CX _sensor;
  VL53L5CX_ResultsData _data;
};

#endif // MULTIZONE_TOF_H