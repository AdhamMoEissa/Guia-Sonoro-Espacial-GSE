#include "Single_Zone_ToF.h"

SingleZoneToF::SingleZoneToF(uint8_t muxChannel, const char* label)
  : _muxChannel(muxChannel), _label(label) {}

SingleZoneToF::SingleZoneToF(const char* label)
  : _muxChannel(NO_MUX_CHANNEL), _label(label) {}

// Initializes the VL53L1X sensor (selecting its mux channel if configured) and starts ranging.
bool SingleZoneToF::init() {
  if (_muxChannel <= 7) {
    selectTCAChannel(_muxChannel);
  }

  if (_sensor.begin() != 0) {
    return false;
  }

  _sensor.setDistanceModeShort(); // Range up to ~1.3m with high ambient light rejection
  _sensor.setTimingBudgetInMs(33);
  _sensor.setInterMeasurementInMs(33);
  _sensor.startRanging();
  return true;
}

bool SingleZoneToF::begin() {
  return init();
}

// Reads a new distance measurement in mm if data is ready.
bool SingleZoneToF::read(uint16_t &distance_mm) {
  if (_muxChannel <= 7) {
    selectTCAChannel(_muxChannel);
  }

  if (!_sensor.checkForDataReady()) {
    return false;
  }

  distance_mm = _sensor.getDistance();
  _sensor.clearInterrupt();  // Clear interrupt so the next measurement can trigger
  return true;
}

const char* SingleZoneToF::getLabel() const {
  return _label;
}