# Solid-State Water Quality Testing Cup
## Project Summary — Teammate Reference Document

> **Purpose:** This document summarises all design decisions, physics, firmware, hardware, and experimental results from the project development chat. Use this as your starting point for continuing work.

---

# SECTION 1 — PROJECT OVERVIEW

## Core Concept
A multi-parameter water quality testing device shaped like a cup. The defining engineering constraint is **100% solid-state, non-contact architecture** — no electrodes, no glass probes, no electrical contact between sensing electronics and the water being tested.

## Why This Architecture
- Eliminates ground loops between sensors when multiple are submerged together
- Eliminates sensor fouling (probes getting dirty)
- Eliminates fragile glass pH bulbs
- Drives unit cost toward mass-manufacturing economics
- Genuinely novel — no prior art found for this specific combination

## Three Sensing Subsystems

| Subsystem | Parameter | Method |
|---|---|---|
| A | Turbidity | RF attenuation / CSI phase (2.4GHz) |
| B | TDS (Total Dissolved Solids) | Dual-wavelength laser refractometry |
| C | pH, Chlorine, Hardness | Optical colorimetry (phenol red dye + RGB sensor) |

---

# SECTION 2 — PATENT STATUS

## Patentability Assessment (India, Patents Act 1970)

| Subsystem | Novelty | Strength |
|---|---|---|
| RF/CSI turbidity method | High — no prior art found | Strongest independent claim |
| Laser refractometry TDS (specific geometry) | Medium — principle known, geometry novel | Strong dependent claim |
| Colorimetric pH (ratiometric spectral) | Low standalone | Part of combination claim |
| Entire cup system (combination) | High | Primary patent claim |

## Key Filing Facts
- **No prototype required** for provisional filing
- Theory, mathematics, and figures are sufficient at provisional stage
- File provisional first (costs ~₹1,600 on IP India portal)
- 12-month window after provisional to build PoC and file complete specification
- **Do not publish or present publicly before filing**

## Provisional Specification Must Include
1. Cross-section diagram of cup with all three subsystems labelled
2. Block diagram of electronics
3. Mathematical description of each sensing method
4. RF attenuation theory + calibration curve concept
5. Snell's Law geometry for refractometry
6. RGB-to-HSV or ratiometric spectral conversion for pH
7. Informal claims list (formal claims required only in complete spec)

## Strongest Patent Claims
1. The cup-form-factor solid-state multi-parameter sensing architecture
2. RF/CSI-based non-contact turbidity sensing through cup walls
3. Dual-wavelength refractometry with mathematical temperature/TDS decoupling
4. Ratiometric spectral pH using dissolved indicator dye

---

# SECTION 3 — HARDWARE ARCHITECTURE

## Microcontrollers

| Board | Role | Notes |
|---|---|---|
| ESP32-C3 Supermini #1 | RF Turbidity — Master/Tx | Transmits ESP-NOW beacons through water |
| ESP32-C3 Supermini #2 | RF Turbidity — Slave/Rx | Measures RSSI / CSI, outputs to Serial Monitor |
| ESP32 DevBoard (38-pin, black) | Master MCU | Runs TDS, colorimetric, OLED, BLE |

## Critical Arduino IDE Settings (Both ESP32-C3 Boards)
```
Board:              ESP32C3 Dev Module
USB CDC On Boot:    Enabled  ← MANDATORY or Serial won't work
Upload Speed:       115200
Flash Mode:         DIO
Partition Scheme:   Default 4MB with spiffs
Core version:       3.0.0 or higher  ← MANDATORY for rx_ctrl->rssi
```

## Known Firmware Issues Fixed
- `BOOT_PIN` conflicts with `esp32-hal.h` → renamed to `BTN_BOOT`
- Send callback must use `wifi_tx_info_t*` not `uint8_t*` in core v3
- No `ARDUINO_ISR_ATTR` on ESP-NOW callbacks
- USB-CDC needs 2–3 second wait loop in `setup()` before Serial output

---

# SECTION 4 — TURBIDITY SUBSYSTEM (RF / CSI)

## Current Status: RSSI Method Has Physics Problems

### What Was Tested
Two ESP32-C3 Superminis mounted on opposite sides of a 5cm diameter PET bottle filled with water. ESP-NOW packets fired at 20/second. RSSI measured from `recv_info->rx_ctrl->rssi` in receive callback.

### Experimental Results
```
Clean water:          -38 dBm baseline (through cup, correct geometry)
Turbid water (flour): -25 dBm (RSSI went UP — wrong direction)
Air (no water):       -29 dBm
```

### Why RSSI Goes The Wrong Direction
At 2.4GHz, **water is the absorber, not the particles.** Adding suspended particles displaces water molecules, reducing bulk dielectric loss of the mixture. More particles = less water in path = less RF absorption = RSSI increases. Particle scattering is negligible because particles (0.1–100µm) are 100–10,000× smaller than the 14.2mm wavelength inside water at 2.4GHz.

### Why Signal Magnitude Is Too Small
At 100 NTU, particles occupy < 0.002% of water volume. The resulting dielectric change is below the ESP32's 1 dBm RSSI resolution floor. This is a fundamental physics constraint, not a fixable engineering problem.

### Physical Setup Problems Found
- Pin headers created air gap between boards and bottle → RF leaked around water
- Reflected paths from desk/laptop/walls dominated over direct through-water path
- No RF shielding on sides of bottle
- Cupping hands helped identify but did not resolve the direction issue

## Three Viable Solutions

### Solution A — Power Sweep Threshold (Implement Now, Same Hardware)
Instead of reading RSSI amplitude, step transmit power down in known increments and find the link margin threshold — the minimum power at which the Slave still receives packets. More turbid water requires higher transmit power to maintain the link.

- Resolution: ~500 NTU (not drinking-water precise)
- Cost: ₹0, same hardware, implementable today
- Patent value: Medium

```cpp
// Master sweeps TX power from max to min
// Slave reports packet reception rate at each power level
// Threshold power = turbidity proxy
const int8_t powerLevels[] = {20, 28, 40, 52, 60, 68, 76, 84}; // ×0.25 dBm
```

### Solution B — LoRa at 868MHz (Better Physics)
At 868MHz, water's dielectric loss is lower, so turbidity signal is relatively larger. Replace ESP32-C3 nodes with SX1276 LoRa modules.
- Cost: ~₹400 for two modules
- Resolution: Better than 2.4GHz, still not NTU-precise
- Patent value: Medium

### Solution C — ESP32 CSI Phase Measurement (Best, Recommended)
**Channel State Information (CSI)** from the ESP32's WiFi radio includes per-subcarrier phase data. Phase changes more than amplitude when turbidity changes because particles affect the real part of permittivity (ε') which RSSI cannot see.

- Requires: Full ESP32 or ESP32-S3 (NOT ESP32-C3 — C3 does not support CSI)
- Your existing black 38-pin ESP32 DevBoard supports CSI
- Resolution: ~100–200 NTU threshold (far better than RSSI)
- Cost: ₹0 — use existing DevBoard
- Patent value: **Strong** — CSI-based liquid turbidity sensing is novel, published research supports the physics

**Recommended next step: Implement CSI on the ESP32 DevBoard.**

## RSSI Firmware (Current Working Code)

### Flashing Order
1. Flash `GET_MAC.ino` to Slave → write down MAC address printed
2. Flash `MASTER.ino` to ESP32-C3 #1 (paste Slave MAC first)
3. Flash `SLAVE.ino` to ESP32-C3 #2
4. Power Master from USB power bank; Slave stays connected to PC

### Key Code Facts
```cpp
// Slave receive callback — RSSI extraction
void onPacketReceived(const esp_now_recv_info_t *recv_info,
                      const uint8_t *data, int len) {
    int8_t rssi = recv_info->rx_ctrl->rssi;  // Range: -120 to 0 dBm
    // Store in ring buffer for median filtering
}

// Master send callback — CORRECT v3 signature
void onPacketSent(const wifi_tx_info_t *info,        // NOT uint8_t*
                  esp_now_send_status_t status) { }

// Both boards must use same channel
const uint8_t WIFI_CHANNEL = 1;  // Change if WiFi congestion on ch.1

// Buffer size increased to 40 (no decoupling caps available)
const uint8_t RSSI_BUFFER_SIZE = 40;
```

### NTU Conversion (After Calibration)
```
NTU = A × RSSI² + B × RSSI + C
```
Coefficients A, B, C are placeholders (0.0) until lab calibration with known NTU standards. RSSI values are real and accurate immediately.

### Temperature Compensation
```
RSSI_compensated = RSSI_raw - (K_TEMP × (T_water - 25.0))
K_TEMP = 0.2 dBm/°C (starting estimate, refine during calibration)
Apply compensation BEFORE polynomial conversion, not after.
```

---

# SECTION 5 — TDS SUBSYSTEM (LASER REFRACTOMETRY)

## Physics

The cup has a 45° diagonal laser tunnel. The laser enters the water perpendicular to the 45° glass coverslip (zero bending at entry). The beam travels at 45° through the water and exits through the opposite cup wall at ~70° from horizontal.

**Key geometric advantage:** At 70° exit angle, the geometric amplification factor is 8.7×, giving ~60µm beam displacement per 1000 ppm TDS at 20mm detector standoff. This is detectable with the BPW34 differential pair.

## Dual-Wavelength Method

Two lasers (650nm red + 405nm violet) travel the same path. They exit at slightly different angles due to chromatic dispersion:
```
Red (650nm):    exit angle = 70.2°
Violet (405nm): exit angle = 71.8°
Spot separation at 20mm standoff: 6.2mm vertical
```

### Why Two Wavelengths
1. **Mechanical noise cancellation:** Both beams shift equally when cup vibrates — their separation is immune to physical disturbance
2. **Temperature/TDS decoupling:** TDS and temperature affect the two wavelengths in different ratios, allowing a 2×2 matrix solve for both simultaneously — no temperature sensor required
3. **Stronger patent claim:** Novel dual-observable ratiometric measurement

### 2×2 Matrix Solution
```
D_red    = α_red    × ΔTDS + β_red    × ΔT
D_violet = α_violet × ΔTDS + β_violet × ΔT

Constants:
α_red    = 1.65×10⁻⁷  (dn/dC at 650nm, per ppm)
α_violet = 1.80×10⁻⁷  (dn/dC at 405nm, per ppm)
β_red    = -9.5×10⁻⁵  (dn/dT at 650nm, per °C)
β_violet = -10.0×10⁻⁵ (dn/dT at 405nm, per °C)
DET      = (α_red × β_violet) - (α_violet × β_red) = 6.0×10⁻¹⁴

Solve by Cramer's Rule:
ΔTDS = (β_violet × dn_red - β_red × dn_violet) / DET
ΔT   = (α_red × dn_violet - α_violet × dn_red) / DET
```

## Detector: BPW34 Differential Pair (Replaces TSL1401CL)

TSL1401CL was not available. BPW34 differential pairs are used instead — actually better for small displacement signals.

```
Two BPW34 diodes mounted vertically (PD_A above PD_B)
Gap between faces: 0.3–0.5mm (use cardstock shim during assembly)
Red cellophane filter over red pair
Violet cellophane filter over violet pair
Gap must be VERTICAL because beam moves vertically with TDS change

Signal: (I_A - I_B) / (I_A + I_B)
Ratiometric — immune to laser power drift
```

## Signal Conditioning Circuit
```
BPW34 → LM358 TIA (transimpedance amplifier)
Feedback resistor: 1MΩ
Stability capacitor: 10pF across feedback resistor
Output voltage: I_photodiode × 1MΩ
ESP32 ADC reads output voltage (GPIO32, 33, 34, 35 — all ADC1)
```

## Coverslip Alignment Test
```
Shine laser through tunnel with coverslip in place (dry, no water)
If reflection goes straight back into laser tunnel → coverslip angle correct ✅
If reflection goes sideways → adjust coverslip angle with paper shim ❌
Perpendicular incidence produces retroreflection
```

## Calibration
Prepare NaCl solutions at 100, 250, 500, 1000, 2000 ppm.
Record differential ratio at each concentration.
Run Python regression to find K_SENS_RED and K_SENS_VIOLET.
Use Cramer's Rule to solve for TDS and temperature simultaneously.

---

# SECTION 6 — pH SUBSYSTEM (COLORIMETRY)

## Method: Phenol Red Dissolved Dye + TCS34725 RGB Sensor

Add a microdrop of phenol red solution to the water sample. The dye changes colour based on pH:
```
pH < 6.8  → Yellow
pH = 7.0  → Orange
pH > 8.4  → Red
```
This range (6.8–8.4) covers the entire WHO drinking water acceptability window exactly.

## Why Not Test Strips
- Strips cost ₹2–5 each; phenol red costs < ₹0.05/test
- Dissolved dye gives uniform optical path; strips have uneven surfaces
- Phenol red shelf life: 3–5 years

## Where to Get Phenol Red in Mumbai
- SRL Chemicals / Byculla chemical market
- HiMedia, Merck India on IndiaMART
- Your college biology/microbiology lab likely has it — ask before buying

## Ratiometric pH Calculation
```cpp
// TCS34725 reads R, G, B, Clear channels
// Normalise against Clear to remove LED intensity dependence
float rNorm = (float)r / (float)c;
float bNorm = (float)b / (float)c;

// Ratio rises monotonically with pH
float ratio = rNorm / (rNorm + bNorm);

// Convert to pH via calibration polynomial
float pH = (CAL_A * ratio * ratio) + (CAL_B * ratio) + CAL_C;
```

Using ratio instead of raw values makes reading immune to LED brightness variation — same principle as HSV hue method.

## Accuracy
- TCS34725 + phenol red + ratiometric: ±0.2–0.4 pH
- Better than test strips (±0.5–1.0)
- Sufficient for WHO 6.5–8.5 drinking water standard

## Calibration Required
pH 4.0, 7.0, and 10.0 buffer sachets (~₹80 each, one-time purchase).
Dissolve phenol red in each buffer at same concentration as your measurement setup.
Record ratio value for each known pH.
Fit 2nd-order polynomial in Python.

---

# SECTION 7 — CUP DESIGN (3D PRINTED)

## Already Built Features
- 45° diagonal laser tunnel (sealed, light-tight)
- Glass coverslip pocket at base of tunnel (45° angle)
- Two rectangular side cutouts for RF node mounting
- Circular colorimetric sensor window on side wall
- Removable base plate for assembly access

## Confirmed Beam Path
- Laser enters water perpendicular to 45° coverslip → no bending at entry
- Beam travels at 45° through water column
- Exits through opposite cup wall at ~70° from horizontal
- All refraction happens at the exit wall — this is where TDS is measured

## Detector Standoff Bracket (To Be Printed)
- Mounts on outside of cup wall opposite laser tunnel
- 20mm standoff from outer wall surface to photodiode faces
- Upper position: violet BPW34 pair
- Lower position: red BPW34 pair
- Vertical separation between pair centres: 6.2mm

## Assembly Sequence
```
1. Install coverslip → verify with retroreflection test
2. Install laser in tunnel → dry fire, confirm beam exits
3. Physical beam trace → mark exit point on cup wall
4. Assemble BPW34 pairs → mount at exit point
5. Wire to LM358 TIA circuit on external perfboard
6. Connect to ESP32 ADC
7. Fill with distilled water → run baseline calibration
8. Fill with known NaCl solutions → derive K_SENS
9. Install RF nodes in side pockets
10. Install TCS34725 + phenol red at colorimetric window
```

---

# SECTION 8 — COMPLETE BILL OF MATERIALS

| # | Component | Spec | Qty | Est. Cost (₹) |
|---|---|---|---|---|
| 1 | ESP32-C3 Supermini | 2.4GHz, ESP-NOW | 2 | 360 |
| 2 | ESP32 DevBoard (black) | 38-pin, dual-core | 1 | 350 |
| 3 | TCS34725 RGB sensor | I2C, IR filter | 1 | 280 |
| 4 | Laser diode, 650nm red | 5mW, 5V, with driver | 1 | 100 |
| 5 | Laser diode, 405nm violet | 5mW, 5V, with driver | 1 | 200 |
| 6 | BPW34 photodiode | Silicon PIN, TO-5 | 4 | 112 |
| 7 | Silicone sealant | Neutral cure, clear | 1 tube | 120 |
| 8 | 100µF electrolytic cap | 10V, radial | 2 | 10 |
| 9 | 100nF ceramic cap | 50V | 4 | 8 |
| 10 | 10µF electrolytic cap | 10V, radial | 4 | 20 |
| 11 | LM358 dual op-amp | DIP-8, single supply | 2 | 24 |
| 12 | Resistor 1MΩ | ±1%, 0.25W | 8 | 8 |
| 13 | Resistor 10kΩ | ±5%, 0.25W | 3 | 3 |
| 14 | Resistor 470Ω | ±5%, 0.25W | 3 | 3 |
| 15 | Resistor 330Ω | ±5%, 0.25W | 5 | 5 |
| 16 | N-MOSFET 2N7000 or BS170 | TO-92 | 3 | 24 |
| 17 | Microscope coverslips | 22×22mm, #1 thickness | 1 pack (50) | 120 |
| 18 | White LED, high CRI | CRI≥90, 5600K | 2 | 30 |
| 19 | Phenol red powder | Lab grade, 5g | 1 | 150 |
| 20 | pH buffer sachet pH 4.0 | Calibration standard | 1 | 80 |
| 21 | pH buffer sachet pH 7.0 | Calibration standard | 1 | 80 |
| 22 | pH buffer sachet pH 10.0 | Calibration standard | 1 | 80 |
| 23 | Clear acrylic sheet, 3mm | Cut to 30×20mm | 1 piece | 30 |
| 24 | 18650 Li-Ion cell | 3.7V, ≥2000mAh | 1 | 200 |
| 25 | TP4056 charging module | With protection | 1 | 35 |
| 26 | MT3608 boost converter | 3.7V→5V adjustable | 1 | 45 |
| 27 | 7semi / AMS1117-3.3V LDO | 1A output | 1 | 15 |
| 28 | OLED display | 0.96", SSD1306, I2C | 1 | 120 |
| 29 | Jumper wires | M-M and M-F, 20cm | 1 set | 60 |
| 30 | Breadboard | 830 tie-points | 1 | 80 |
| 31 | Perfboard | 5×7cm | 2 | 50 |
| 32 | 5-minute epoxy | Two-part, clear | 1 pack | 80 |
| 33 | M2 screws and nuts | 6mm | 1 pack | 30 |
| 34 | NaCl (table salt) | Food grade | 1 pack | 20 |
| 35 | Kaolin clay | Fine powder | 100g | 50 |
| 36 | Distilled water | 1 litre | 2 bottles | 50 |
| | **TOTAL** | | | **~₹3,050** |

## LDO Note
AMS1117-3.3V and 7semi 5V→3.3V 1A LDO are functionally identical for this application. Use whichever is available. Neither is needed for the turbidity prototype (both C3 Superminis self-regulate from USB). Only needed when full sensor suite is connected to DevBoard.

## Capacitor Note
100pF capacitors (currently available) are useless for power decoupling — too small by 3–6 orders of magnitude. Work without capacitors for now by:
- Powering Master and Slave from separate USB sources
- Increasing RSSI_BUFFER_SIZE to 40 in firmware
- Order 100µF and 100nF caps with next component batch

---

# SECTION 9 — POWER ARCHITECTURE

```
18650 Li-Ion (3.7V)
        │
        ├──→ TP4056 (charging)
        │
        ├──→ MT3608 Boost → 5V rail → Lasers + White LED
        │                             (gated via MOSFET)
        │
        └──→ ESP32 DevBoard VIN → onboard AMS1117 → 3.3V
                                   → Sensors (TCS34725, OLED, BPW34 circuit)
```

ESP32 DevBoard has its own onboard LDO. External LDO only needed for a dedicated clean analog sensor rail separate from the DevBoard's noisy WiFi-sharing rail.

---

# SECTION 10 — CALIBRATION PROCEDURES

## Turbidity (RF — After Physics Solution is Chosen)
1. Prepare kaolin clay suspensions at 0, 50, 100, 250, 500 NTU
2. Get reference NTU values certified by a water testing lab (CPCB lab)
3. Record RSSI (median) for each standard at 25°C
4. Run Python polynomial regression → get A, B, C
5. Determine K_TEMP by measuring RSSI at 10°C, 15°C, 25°C, 35°C with same sample

## TDS (Refractometry)
1. Prepare NaCl solutions: 100, 250, 500, 1000, 2000 ppm
2. Verify concentrations with a TDS pen meter (₹300)
3. Record differential ratio (red and violet) for each
4. Run Python regression → get K_SENS_RED, K_SENS_VIOLET
5. Verify temperature decoupling by testing at 15°C and 35°C

## pH (Colorimetry)
1. Add phenol red to pH 4.0, 7.0, 10.0 buffer solutions at standard concentration
2. Read TCS34725 R, G, B, C for each
3. Compute ratio = rNorm / (rNorm + bNorm)
4. Fit 2nd-order polynomial ratio → pH
5. Get CAL_A, CAL_B, CAL_C

## Python Regression Script (All Subsystems)
```python
import numpy as np
from sklearn.metrics import r2_score

# Replace with your actual data
x = np.array([-55, -62, -68, -75, -85])  # RSSI or ratio values
y = np.array([0, 50, 100, 250, 500])      # Known NTU or ppm or pH

coeffs = np.polyfit(x, y, 2)  # 2nd-order polynomial
y_pred = np.polyval(coeffs, x)
r2     = r2_score(y, y_pred)

print(f"A={coeffs[0]:.6f}, B={coeffs[1]:.6f}, C={coeffs[2]:.6f}")
print(f"R² = {r2:.4f}  (target > 0.95)")
# Paste A, B, C into firmware
```

---

# SECTION 11 — PATENT FILING GUIDE

## Immediate Action: File Provisional on IP India Portal
**Website:** https://ipindiaonline.gov.in

**Cost:** ~₹1,600 for individual applicant (e-filing)

**Forms needed:**
- Form 1: Application for Grant of Patent (applicant details, inventor details)
- Form 2: Provisional Specification (your description document)

**What to include in provisional specification:**
1. Title (technical, not marketing)
2. Background (problem with existing electrochemical sensors)
3. Summary (three-subsystem non-contact architecture)
4. Figure descriptions (cross-section, block diagram, calibration curves)
5. Detailed description of each subsystem with physics and math
6. Informal claims list

**12-month window after provisional:**
- Month 1–3: Build and validate prototype
- Month 5–7: Write complete specification
- Month 7–8: Patent Agent reviews and drafts formal claims
- Before Month 12: File complete specification (Form 2, complete)

**Patent Agent fee:** ₹15,000–₹40,000 for formal claim drafting
**Find agents at:** https://ipindiaonline.gov.in/patentSearch/searchAgentPatent.aspx

---

# SECTION 12 — CURRENT EXPERIMENTAL RESULTS

## Turbidity RF Test (Completed)

| Condition | RSSI Reading | Notes |
|---|---|---|
| Clean water (bottle, correct geometry) | -38 dBm | Good baseline, strong signal |
| Turbid water with flour | -25 dBm | RSSI went UP — physics problem |
| Air baseline | -29 dBm | Less attenuation than water as expected |

**Conclusion:** RSSI method confirmed not suitable for drinking water turbidity range. CSI phase method recommended as replacement.

## Packet Statistics
- Drop rate in initial test: 9.2% — too high, switch from channel 1 to channel 6 or 11
- Drop rate after bench test: 2.7% — acceptable
- RSSI stability: ±2 dBm with buffer=40, no capacitors — good

---

# SECTION 13 — NEXT STEPS (PRIORITY ORDER)

## Immediate
1. **Implement CSI on ESP32 DevBoard** for turbidity — replaces RSSI method
2. **Change WiFi channel from 1 to 6** in both Master and Slave firmware to reduce packet drop rate
3. **Source IR LED (850nm, ₹15)** as backup optical turbidity method

## Short Term
4. Build BPW34 differential pair detector bracket for TDS subsystem
5. Install laser modules and coverslip in 3D printed cup
6. Physical beam trace to find exit point
7. Run TDS calibration with NaCl solutions

## Medium Term
8. Build phenol red + TCS34725 pH subsystem
9. Calibrate pH with buffer solutions
10. Full system integration test
11. File provisional patent

## Before Patent Filing
12. Collect calibration curves for all three subsystems
13. Run validation against reference instruments
14. Take professional photographs of assembled device
15. Draw formal cross-section figures with reference numerals

---

# SECTION 14 — RESOURCES AND CONTACTS

## Component Sources (Mumbai)
| Item | Where |
|---|---|
| Electronics components | Robu.in, Lamington Road, SP Road |
| Phenol red powder | Byculla chemical market, SRL/HiMedia on IndiaMART |
| pH buffer sachets | Same as above, or college chemistry department |
| Kaolin clay | Pottery suppliers near Dharavi/Kurla |
| Distilled water | Any pharmacy |
| Microscope coverslips | Scientific suppliers near CSMT or college lab |

## Key Reference Links
- IP India patent filing: https://ipindiaonline.gov.in
- Patent agent search: https://ipindiaonline.gov.in/patentSearch/searchAgentPatent.aspx
- ESP32 Arduino core (v3.x required): https://github.com/espressif/arduino-esp32
- ESP-NOW documentation: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/network/esp_now.html

## Funding Opportunities After Patent Filing
- DST NIDHI PRAYAS Grant: ₹10 lakh for student prototype development
- Atal Innovation Mission (AIM): Incubation + seed funding
- SINE IIT Bombay: Technology transfer and startup incubation
- BIRAC SITARE: Health/environment tech student startups

---

*Document generated from project development conversation. All physics, firmware, calibration procedures, and experimental results reflect work completed as of the conversation date. Teammate continuing this work should start from Section 13 (Next Steps) and refer back to relevant sections as needed.*
