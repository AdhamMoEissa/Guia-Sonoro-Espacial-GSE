# SpatialAssist: 360° Wearable Spatial Audio Navigation & Wireless Telemetry System

An assistive embedded navigation system designed for visually impaired individuals, paired with a wireless companion terminal. The wearable unit fuses optical Time-of-Flight (ToF), ultrasonic echo dynamics, and inertial orientation tracking to translate 360° environmental hazards into quadraphonic open-ear acoustic cues. Telemetry is simultaneously transmitted via low-latency peer-to-peer wireless protocol to a handheld caregiver/observer terminal that renders real-time graphical polar radar sweeps and textual diagnostic data.

---

## 📖 Overview

Navigating dynamic environments presents hazardous challenges for visually impaired individuals, particularly head- and chest-height obstacles (e.g., low-hanging branches, open doors, signs) that traditional white canes miss. 

**SpatialAssist** addresses this through a distributed two-node embedded architecture:
1. **Wearable Sensor Headwear (Node A):** An ESP32-driven sensing and audio processing platform mounted on a hat. It samples multiple physical phenomena simultaneously, compensates for user head tilt, and synthesizes 4-channel open-ear spatial audio.
2. **Handheld Telemetry Terminal (Node B):** A secondary ESP32 device equipped with an SPI TFT-LCD display. Acting as a sighted-guide or caregiver monitoring unit, it receives real-time distance vectors wirelessly and presents them through graphical 2D radar plots and text telemetry.

---

## 🔬 Multi-Phenomena Sensing & Academic Compliance

The system directly measures three distinct physical phenomena via dedicated sensor hardware:

1. **Photonic Propagation (Optical Time-of-Flight):** Measures round-trip transit time of $940\text{ nm}$ infrared laser photons to resolve distances with millimeter precision across front, diagonal, and rear sectors.
2. **Acoustic Pressure Waves (Ultrasound):** Measures round-trip acoustic pulse reflections at $40\text{ kHz}$ across lateral flanks, detecting optically transparent surfaces (e.g., glass doors and clear partitions) that absorb or refract infrared light.
3. **Inertial Kinematics (Gravitational Acceleration & Angular Velocity):** Samples dynamic gravitational vectors and Coriolis forces via a 6-axis MEMS accelerometer/gyroscope to determine head pitch and roll, filtering out false floor-plane collisions.

---

## ✨ Key Features

* **Distributed Wireless Architecture:** Low-latency peer-to-peer telemetry between Wearable Node A and Observer Node B using **ESP-NOW** ($< 5\text{ ms}$ packet latency, zero external Wi-Fi router dependencies).
* **Dual Graphical & Textual Visualization:**
  * **Graphical Display:** Real-time 360° polar radar grid mapping proximity thresholds with dynamic color gradients (Green $\rightarrow$ Yellow $\rightarrow$ Red).
  * **Textual Display:** Instantaneous sector readouts (cm), active safety alerts, head orientation (pitch/roll degrees), and wireless link signal strength (RSSI).
* **True Quadraphonic Open-Ear Feedback:** Four micro-drivers placed around the ears leverage natural pinna filtering for distinct front/back acoustic localization while keeping ear canals unoccluded.
* **Cognitive-Safe Sound Synthesis:**
  * **Silence-by-Default:** Zero audio output when clearance exceeds $1.8\text{ m}$.
  * **Pentatonic Harmonic Chimes:** Quantized musical plucks prevent dissonance and cognitive fatigue.
* **Head-Tilt Ground Filtering:** Automatically suppresses frontal ground false alarms when the user looks downward.

---

## 🛠 System Architecture

```
+---------------------------------------------------------------------------------------+
|                                WEARABLE HEADWEAR (NODE A)                             |
|                                                                                       |
|   +-------------------+    +--------------------+    +----------------------------+   |
|   |  ST VL53L5CX ToF  |    |  3x ST VL53L1X ToF |    |   2x RCWL-1601 Ultrasonic  |   |
|   |  (Front 8x8 Grid) |    |  (Angles & Rear)   |    |      (Left & Right)        |   |
|   +---------+---------+    +---------+----------+    +--------------+-------------+   |
|             |                        |                              |                 |
|             +------------+-----------+                              |                 |
|                          | I2C (via TCA9548A Mux)                   | GPIO Trig/Echo  |
|                          v                                          v                 |
|                 +-------------------------------------------------------+             |
|                 |                 ESP32 PRIMARY MCU                     |             |
|                 |                                                       |             |
|                 |  [Core 0: Sensing]        [Core 1: Audio Engine]      |             |
|                 |   - Multi-Sensor Polling   - Pentatonic Synthesizer   |             |
|                 |   - IMU Tilt Correction    - Dual I2S Driver          |             |
|                 |   - ESP-NOW Broadcast                                 |             |
|                 +--------+------------------------------+---------------+             |
+--------------------------|------------------------------|-----------------------------+
                           |                              | Dual I2S (4 Channels)
                           |                              v
                           |                   +----------------------+
                           |                   |  4x MAX98357A Amps   |
                           |                   |  + 4x Micro Speakers |
                           |                   +----------------------+
                           |
                           | 2.4 GHz ESP-NOW Wireless Packets
                           | (~20 Hz Refresh Rate)
                           v
+---------------------------------------------------------------------------------------+
|                            HANDHELD TERMINAL (NODE B)                                 |
|                                                                                       |
|   +-------------------------------------------------------------------------------+   |
|   |                              ESP32 RECEIVER MCU                               |   |
|   |                                                                               |   |
|   |   - ESP-NOW Packet Ingestion & CRC Verification                               |   |
|   |   - Graphical Polar Radar Buffer Renderer                                     |   |
|   |   - Textual UI Engine                                                         |   |
|   +---------------------------------------+---------------------------------------+   |
|                                           | SPI Bus                                   |
|                                           v                                           |
|   +-------------------------------------------------------------------------------+   |
|   |                          2.8" SPI TFT-LCD DISPLAY                             |   |
|   |                                                                               |   |
|   |   [ Graphical Polar Radar ]                  [ Real-Time Text Readout ]       |   |
|   |            FRONT                             FRONT : 142 cm   [CLEAR]         |   |
|   |              ▲                               F-LEFT:  48 cm   [WARNING]       |   |
|   |         /    |    \                          F-RGHT:  85 cm   [ALERT]         |   |
|   |    LEFT ─── (o) ─── RIGHT                    LEFT  : >200cm   [CLEAR]         |   |
|   |         \    |    /                          RIGHT : 110 cm   [CLEAR]         |   |
|   |              ▼                               REAR  : >200cm   [CLEAR]         |   |
|   |             REAR                             PITCH : -4.2°    ROLL: 1.1°      |   |
+---------------------------------------------------------------------------------------+
```

---

## 📡 Wireless Communication Protocol (ESP-NOW)

Node A and Node B communicate using the connectionless **ESP-NOW** protocol operating on $2.4\text{ GHz}$ ISM band channels:

* **Payload Structure (`TelemetryPacket`):**
  ```cpp
  typedef struct __attribute__((packed)) {
      uint32_t timestamp;        // Milliseconds since boot
      uint16_t dist_front;       // Center front distance (cm)
      uint16_t dist_front_left;  // Diagonal front-left (cm)
      uint16_t dist_front_right; // Diagonal front-right (cm)
      uint16_t dist_left;        // Lateral left ultrasonic (cm)
      uint16_t dist_right;       // Lateral right ultrasonic (cm)
      uint16_t dist_rear;        // Rear ToF distance (cm)
      float head_pitch;          // Pitch angle from IMU (degrees)
      float head_roll;           // Roll angle from IMU (degrees)
      uint8_t alert_flags;       // Bitfield of active collision zones
  } TelemetryPacket;
  ```
* **Performance:** Packets are transmitted at $20\text{ Hz}$ with zero Wi-Fi handshake overhead, ensuring the visual terminal mirrors user motion in real time without interfering with audio generation.

---

## 🖥 Display Interface (Handheld Node B)

The companion unit features an SPI TFT display (e.g., ST7789 or ILI9341, $320\times240$ resolution) partitioned into two distinct viewports:

1. **Graphical Sweep Viewport (Left Half):**
   * Polar radar map with concentric rings at $0.5\text{ m}$, $1.0\text{ m}$, and $1.8\text{ m}$.
   * Dynamic obstacle blips plotted by vector angle and distance.
   * Color-coded threat levels: Green (Clear, $> 1.5\text{ m}$), Yellow (Caution, $0.8\text{–}1.5\text{ m}$), Red (Hazard, $< 0.8\text{ m}$).
2. **Textual Telemetry Viewport (Right Half):**
   * Numeric readouts for all 6 sensory channels in centimeters.
   * Head attitude readouts ($\text{Pitch}$, $\text{Roll}$) indicating user gaze vector.
   * System status banners: `"CLEAR"`, `"HEAD-HEIGHT WARNING"`, `"LATERAL CLEARANCE LOW"`.

---

## 🔊 Audio Feedback Scheme

### Driver Placement & Psychoacoustics
* **Front-Left & Front-Right:** Temple-mounted, angled rearward toward the ear canal opening.
* **Rear-Left & Rear-Right:** Sweatband-mounted, angled toward the rear surface of the pinna.
* **Acoustic Pinna Cues:** Sound from the rear drivers reflects off the posterior ear flap, providing physical front-to-back discrimination without requiring synthetic HRTF DSP overhead.

### Acoustic Modulation
* **Safe Zone ($> 1.8\text{ m}$):** Silence.
* **Warning Zone ($0.8\text{ m} - 1.8\text{ m}$):** Soft pentatonic plucks ($C - D - E - G - A$). Repetition rate scales inversely with distance:
  $$t_{\text{interval}} = k \cdot d$$
* **Critical Collision Zone ($< 0.8\text{ m}$):** Crisp, high-attack transient chimes.

---

## 📋 Bill of Materials (BOM)

### Primary Wearable Unit (Node A)
| Component | Description | Qty |
| :--- | :--- | :---: |
| **Microcontroller** | ESP32-WROOM-32 Development Board | 1 |
| **Multizone ToF** | ST VL53L5CX ($8\times8$ Grid Breakout) | 1 |
| **Single-Zone ToF** | ST VL53L1X Breakout Modules | 3 |
| **Ultrasonic Sensors** | RCWL-1601 or US-016 (3.3V Native) | 2 |
| **Bus Multiplexer** | TCA9548A 8-Channel $I^2C$ Multiplexer | 1 |
| **Motion Tracking** | MPU-6050 6-Axis IMU Module | 1 |
| **Audio Amplifiers** | MAX98357A Mono $I^2S$ Class-D Modules | 4 |
| **Speakers** | 2030 Box Micro-Speakers or 15.4 mm Dynamic Drivers ($8\Omega\text{ to }32\Omega$) | 4 |
| **Power Source** | Slim 5V USB Power Bank (3000–5000 mAh) | 1 |
| **Cabling** | 10-Conductor 28 AWG Flat Ribbon Cable | 1–2 m |

### Handheld Monitor Terminal (Node B)
| Component | Description | Qty |
| :--- | :--- | :---: |
| **Microcontroller** | ESP32-WROOM-32 Development Board | 1 |
| **Display** | 2.4" or 2.8" SPI TFT-LCD Module (ILI9341 / ST7789, $320\times240$) | 1 |
| **Power Source** | Portable 5V USB Power Bank or LiPo Battery Pack | 1 |

---

## ⚡ Pinout Mapping

### Node A (Wearable Unit)
| ESP32 Pin | Function | Peripheral Target |
| :--- | :--- | :--- |
| **GPIO 21** | `I2C_SDA` | TCA9548A Multiplexer SDA |
| **GPIO 22** | `I2C_SCL` | TCA9548A Multiplexer SCL |
| **GPIO 25** | `I2S0_BCLK` | Bit Clock (Front Amplifiers) |
| **GPIO 26** | `I2S0_LRC` | Word Select (Front Amplifiers) |
| **GPIO 27** | `I2S0_DOUT` | Data Out (Front Amplifiers) |
| **GPIO 16** | `I2S1_BCLK` | Bit Clock (Rear Amplifiers) |
| **GPIO 17** | `I2S1_LRC` | Word Select (Rear Amplifiers) |
| **GPIO 4** | `I2S1_DOUT` | Data Out (Rear Amplifiers) |
| **GPIO 18 / 19**| `TRIG_L` / `ECHO_L` | Left Ultrasonic Transducer |
| **GPIO 14 / 15**| `TRIG_R` / `ECHO_R` | Right Ultrasonic Transducer |

### Node B (Handheld Display Unit)
| ESP32 Pin | SPI Function | Display Target (ILI9341 / ST7789) |
| :--- | :--- | :--- |
| **GPIO 18** | `SPI_SCK` | Display Clock (`SCK` / `CLK`) |
| **GPIO 23** | `SPI_MOSI` | Display Data (`MOSI` / `SDA`) |
| **GPIO 15** | `TFT_CS` | Chip Select (`CS`) |
| **GPIO 2**  | `TFT_DC` | Data / Command (`DC`) |
| **GPIO 4**  | `TFT_RST` | Hardware Reset (`RST`) |

---

## 💻 Repository Structure

```
├── firmware/
│   ├── node_a_wearable/             # Wearable headwear firmware
│   │   ├── src/
│   │   │   ├── main.cpp             # FreeRTOS setup & tasks
│   │   │   ├── config.h             # Pins, calibration & thresholds
│   │   │   ├── wireless_tx.cpp      # ESP-NOW transmitter logic
│   │   │   ├── sensors/             # Drivers for ToF, Ultrasonic & IMU
│   │   │   └── audio/               # Dual-I2S spatial sound generation
│   │   └── platformio.ini
│   │
│   └── node_b_terminal/             # Handheld monitor terminal firmware
│       ├── src/
│       │   ├── main.cpp             # Ingestion & rendering engine
│       │   ├── wireless_rx.cpp      # ESP-NOW receiver callback
│       │   ├── display_radar.cpp    # 2D polar radar renderer
│       │   └── display_text.cpp     # Formatted metrics & alert banners
│       └── platformio.ini
├── hardware/
│   ├── schematics/                  # Schematics for Node A and Node B
│   └── 3d_prints/                   # Enclosures for terminal & hat brackets
└── docs/                            # Project reports & rubrics
```

---

## 🚀 Getting Started

### Prerequisites
* [PlatformIO IDE](https://platformio.org/) installed in VS Code.
* Required Libraries:
  * **Node A:** `SparkFun VL53L5CX`, `Pololu VL53L1X`, `Adafruit MPU6050`
  * **Node B:** `TFT_eSPI` (configured for ILI9341 or ST7789)

### Flashing Procedure
1. **Flash Node B (Monitor) First:**
   * Open `firmware/node_b_terminal/` in PlatformIO and run `Upload`.
   * Note Node B's MAC address printed to the Serial Monitor ($115200\text{ baud}$).
2. **Configure & Flash Node A (Wearable):**
   * Paste Node B's MAC address into `node_a_wearable/src/config.h` under `RECEIVER_MAC_ADDRESS`.
   * Open `firmware/node_a_wearable/` and run `Upload`.
3. Power both units. Node B will establish a wireless peer-to-peer connection with Node A and begin rendering telemetry immediately.

---

## 📜 License & Academic Integrity

Developed for an Electrical Engineering Senior Capstone Course. Open source under the [MIT License](LICENSE).