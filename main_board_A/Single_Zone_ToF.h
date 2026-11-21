#ifndef SINGLE_ZONE_TOF_H
#define SINGLE_ZONE_TOF_H

#include <Arduino.h>
#include <SparkFun_VL53L1X.h>
#include "TCA_Mux.h"

#ifndef NO_MUX_CHANNEL
#define NO_MUX_CHANNEL 255
#endif

// Wrapper for a VL53L1X single-zone ToF sensor (direct or behind a TCA9548A mux).
class SingleZoneToF {
public:
  SingleZoneToF(uint8_t muxChannel, const char* label);
  SingleZoneToF(const char* label);

  bool init();
  bool begin();
  bool read(uint16_t &distance_mm);
  const char* getLabel() const;

private:
  uint8_t _muxChannel;
  const char* _label;
  SFEVL53L1X _sensor;
};

#endif // SINGLE_ZONE_TOF_H