#include "Multizone_ToF.h"

MultizoneToF::MultizoneToF(uint8_t muxChannel)
  : _muxChannel(muxChannel) {
}

// Initializes the VL53L5CX sensor (selecting its mux channel if configured) and starts 8x8 ranging.
bool MultizoneToF::init() {
  if (_muxChannel <= 7) selectTCAChannel(_muxChannel);

  if (!_sensor.begin()) return false;

  _sensor.setResolution(8 * 8);     // 64 zones
  _sensor.setRangingFrequency(15);  // 15 Hz sample rate
  _sensor.startRanging();
  return true;
}

bool MultizoneToF::begin() {
  return init();
}

// Scans all 64 zones and outputs the closest valid target.
bool MultizoneToF::read(MultizoneTarget &result) {
  if (_muxChannel <= 7) {
    selectTCAChannel(_muxChannel);
  }

  result.detected = false;
  result.minDistance_mm = 4000;  // Max range cap (4 m)
  result.zoneIndex = 255;

  if (!_sensor.isDataReady()) {
    return false;
  }

  if (!_sensor.getRangingData(&_data)) {
    return false;
  }

  for (uint8_t i = 0; i < 64; i++) {
    uint8_t status = _data.target_status[i];
    int16_t dist = _data.distance_mm[i];

    // Status 5 (target detected) and 9 (high confidence) indicate valid returns; ignore < 20 mm noise
    if ((status == 5 || status == 9) && dist > 20 && dist < result.minDistance_mm) {
      result.minDistance_mm = dist;
      result.zoneIndex = i;
      result.row = i / 8;
      result.col = i % 8;
      result.detected = true;
    }
  }

  return result.detected;
}