#ifndef TCA_MUX_H
#define TCA_MUX_H

#include <Arduino.h>
#include <Wire.h>

#define TCA9548A_DEFAULT_ADDR 0x70

// Selects an active I2C channel (0-7) on the TCA9548A multiplexer.
inline void selectTCAChannel(uint8_t channel, uint8_t muxAddr = TCA9548A_DEFAULT_ADDR) {
  if (channel > 7) return;
  Wire.beginTransmission(muxAddr);
  Wire.write(1 << channel);  // Bitmask enables only the target channel
  Wire.endTransmission();
}

#endif // TCA_MUX_H