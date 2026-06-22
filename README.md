# Aurum MAKERMANIA 2026

## Innovation Project Workbook

> Program Duration: 1 June 2026 – 4 July 2026
>
> Location: MBF Tinkerers' Lab 007
>
> Team Size: 3–5 Students
>
> Goal: Identify a real-world problem and develop an innovative, patentable, and implementable solution.

---

# 1. Team Identity

## 1.1 Team Name and Photo
<img width="1280" height="720" alt="WhatsApp Image 2026-06-06 at 12 39 31 PM" src="https://github.com/user-attachments/assets/1b4aa736-58df-420b-bf1f-398b5489473d" />

---

## 1.2 Team Members

| Name | Role | Skills |
| ---- | ---- | ------ |
| Ayush Likhar | Collaborator | Hardware |
| Kartik Phadale | Collaborator | Software |
| Meezaan Malik | Collaborator | Design |

---
# Useless Product Link
https://youtu.be/-Jjdkq18U_4
---
# SCAMPER PROJECT 
https://1drv.ms/p/c/c6b1c4d0bfa1a2d2/IQAcbvFI4a4YR6u9E6l7ya9vAZwWg2kD-QSHdWZtFSUPwtM?e=thSRv1
---
# 2. Problem Discovery

## 2.1 Observation Area

Where did you conduct your observations?

* Hostel
* Canteen
* Workshop
* Hospital
* Public Transport
* Home
* Other

---

## 2.2 AEIOU Observation Sheet

### Activities

What are users doing?
Filling drinking water from tap, borewell, or overhead tank before consuming
Storing water in containers; pots, buckets, plastic cans for daily household use
Manually checking water appearance against light or smell before use
Operating water purifiers (RO/UV) with little understanding of output quality
Buying packaged drinking water due to distrust of tap supply

### Environment

What conditions affect them?
Urban apartments and peri-urban areas with inconsistent municipal supply quality
Rural homes relying on borewell or hand-pump water with unknown contamination
Post-monsoon periods; sediment and biological load spike in surface water
Kitchens and storage areas; device must survive humidity, soap splashes
Power outages common; device cannot depend on mains supply during testing

### Interactions

Who or what are they interacting with?
Water source; tap, tanker, borewell, stored container
Household members; mother making decisions for children's drinking water
Water purifier (RO/UV); users trust it blindly without verifying output
Municipality or tanker supplier; complaints made verbally, no data to back them
Neighbours and community; informal word-of-mouth about water quality

### Objects

What tools or products are used?
Plastic storage containers and overhead tanks, primary water holding vessels
RO/UV purifiers: trusted but unmonitored for actual output quality
TDS pen meters: used by some urban households, numeric output not understood
Packaged water bottles: costly substitute when tap water is distrusted
No pH or turbidity tools in a typical household: gap this device fills
### Users

Who are the primary users?
Homemakers (25–55 yrs); primary decision-makers for household drinking water, non-technical
Renters in urban areas; no control over plumbing, high need to verify supplied water
Rural households; borewell-dependent, no access to lab testing, low literacy
Parents with young children : heightened concern, willing to act on safety signals
Small food businesses: tea stalls, tiffin centres needing quick daily water checks

---

## 2.3 Observation Log

| Observation | Evidence | Pain Point |
| ----------- | -------- | ---------- |
|             |          |            |
|             |          |            |

---

# 3. User Research

## 3.1 Interview Summary

Number of users interviewed: ______

## 3.2 Key Quotes

1.

2.

3.

---

## 3.3 User Persona

### Name

### Age

### Occupation

### Goals

### Frustrations

### Needs

---

# 4. Problem Framing

## Problem Statement

Households users need a way to check water quality because contaminated water can cause serious health risks.

---

## How Might We Questions

1.

2.

3.

---

## Opportunity Ranking

| Criteria         | Score |
| ---------------- | ----- |
| Severity         |       |
| Frequency        |       |
| Feasibility      |       |
| Novelty          |       |
| Market Potential |       |
| Total            |       |

---

# 5. Solution Ideation

## Brainstormed Ideas

| Idea | Advantages | Challenges |
| ---- | ---------- | ---------- |
| Arterial Aging Ring  | Can detect arterial health, blockages  | Miniature, Accuracy |
| Compression Stockings     | Varicose Veins Relief           | Graduated compression accuracy, cost-effective production |
| Trash Compactor | Increased Storage Efficiency, Odour Removal  | Proof of Concept |
| River Cleaaning UAV | Pilotless water body cleaning | Limited large-scale capacity, autonomous navigation challenges |
| Touch Sensing Glove with Haptics | Brings Realistic Feedback | Application and comfort Challenges |
| Tamperproof Weighbridge | Ensures fraud free loading | System Integration |

---

## Selected Concept

Why was this concept chosen?

---

# 6. System Design

## High-Level Description

The solution provides a cup-based turbidity testing system that accepts a water sample, measures its cloudiness through an embedded sensor, and displays or transmits the resulting water quality assessment.

---

## Block Diagram

Insert diagram here.

---

## Inputs

|Input | Importance |
|---- |------ |
| Water Sample in Cup | Main physical input |  
| Colorimetry | pH Value    |
| RGB Sensor | pH level determination |
| nRF24L01  | Transmission of EM waves |

---

## Outputs

| Sensors | Output |
|----- |----- |
|LCD / Display Output | Turbidity reading, clarity percentage, status (clear, cloudy, dirty) |
|Buzzer | Alert |

---

# 7. Technical Planning

## Electronics

| Component | Purpose |
| --------- | ------- |
| ESP32 NodeMCU Development Board | System timing, nRF24 SPI interface, I2C polling, hardware timers for optical tracking |
| ADS1115 16-Bit I2C ADC Module | Overcomes the noisy, non-linear internal ADC of the ESP32 to capture microvolt changes from the custom pH sensor |
| Double-Sided Copper-Clad PCB Board Blank | Slashed into small rectangles to create the physical sensing pad and reference pad surfaces.  |
| Concentrated Sodium Hypochlorite (Bleach) + Sandpaper | Used as an oxidizer on the copper track to grow the reactive copper-oxide (CuO) sensing layer|
| 5V 650nm Red Laser Diode (6mm diameter, 5mW) |Projects a tight optical beam through the diagonal entry tunnel. |
| Glass Microscope Coverslips (0.13 - 0.17mm thick) | Acts as the optically flat, scratch-resistant window sealed into the 45-degree pocket |
| TSL1401CL Linear Photodiode Array (128 Pixels) | Captures the high-resolution spatial displacement of the laser beam spot down to fractional subpixels |
| Dual Light-Dependent Resistors (LDRs) + Razor Blade | Ultra-scalable alternative: Two matching LDRs separated by an opaque blade edge to form a basic analog differential balance pair |
| 2x nRF24L01+ 2.4 GHz Transceiver Modules | Mounted entirely outside the cup walls to measure signal amplitude drop (RSSI) through the liquid column.|
| 2x 2.4 GHz PCB Directional Patch Antennas | Focuses the RF energy straight through the liquid column, minimizing external multipath interference |
| DS18B20 Waterproof Temperature Probe | Real-time thermal tracking to dynamically recalculate the temperature-dependent Nernst slope for the pH firmware|
| Assorted Resistors (10kΩ, 4.7kΩ), 0.1µF Capacitors | Pull-up resistors for the 1-Wire temperature bus, voltage dividers for LDR paths, and decoupling caps for RF rails |
| Clear Marine-Grade Silicone RTV or 2-Part Transparent Epoxy | Creates a reliable waterproof seal around the glass coverslip insert and wire pass through tunnels |
| Turbidity Sensor with Module | Measures water clarity by dropping an infrared emitter/receiver probe directly into the liquid.        |
| pH-4502C Liquid pH Sensor Module + E-201-C Glass Combination Electrode | Measures hydrogen ion activity using a traditional glass bulb probe with a BNC connector interface board 
| TP4056 , Lipo cells |      |

---
## Software

| Tool | Purpose |
| ---- | ------- |
| KiCAD     |  PCB Design       |
| OpenSCAD     | 3D Model Design        |
| BambuLab     | 3D print  |

---

## Mechanical / CAD

Describe fabricated components.

---

# 8. Prototype Development

## Version 1

Description:

Lessons Learned:

---

## Version 2

Description:

Lessons Learned:

---

## Final Prototype

Description:

---

# 9. Testing & Validation

## Testing Plan

| Test | Success Criteria |
| ---- | ---------------- |
|      |                  |
|      |                  |

---

## User Feedback

| User | Feedback | Action Taken |
| ---- | -------- | ------------ |
|      |          |              |

---

# 10. Innovation Assessment

## Existing Solutions

List competing products.

---

## What Makes This Different?

---

## Innovation Score

| Parameter       | Score |
| --------------- | ----- |
| Novelty         |       |
| Technical Depth |       |
| Feasibility     |       |
| Impact          |       |
| Scalability     |       |

---

# 11. Intellectual Property

## Prior Art Search

Patents / Products Found:

---

## Novel Features

1.

2.

3.

---

## Provisional Patent Draft

### Title

### Abstract

### Problem

### Solution

### Claims

---

# 12. Business & Deployment

## Target Users

---

## Estimated Cost

---

## Market Opportunity

---

## Sustainability Considerations

---

# 13. Final Demonstration

## Prototype Images

Insert photos.

---

## Demonstration Video Link

---

## GitHub Repository

---

## Presentation Link

---

# 14. Reflection

## What Worked Well?

---

## What Failed?

---

## Key Learnings

---

## Next Steps

* Patent Filing
* Startup Exploration
* Product Development
* Research Publication
* Competition Submission

---

# 15. Final Deliverables Checklist

* Problem Discovery Complete
* User Interviews Complete
* Persona Created
* Problem Statement Finalized
* System Design Complete
* Prototype Demonstrated
* Testing Completed
* Patent Draft Prepared
* Presentation Submitted
* GitHub Repository Updated

---

# MAKERMANIA FINAL PITCH

Each team will present:

1. Problem
2. User Research
3. Insights
4. Solution
5. Prototype Demo
6. Innovation & Patentability
7. Future Roadmap

Presentation Time: 5 Minutes

Q&A: 3 Minutes
