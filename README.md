# Automated-scribing-machine
An automated linear-guide and pneumatic-clamping machine designed to precisely scribe load cells for strain gauge bonding

# Automated Scribing Machine Development (Industrial Internship Project)

An automated high-precision scribing solution developed during my industrial engineering internship at **Flintec Transducers (Pvt) Ltd**, designed to replace manual digital-vernier marking for load cell strain gauge placement.

---

## Project Background & Problem Statement

In precision load cell manufacturing, strain gauges must be bonded onto exact symmetrical coordinates with a tolerance of **±0.005 mm**. The legacy process relied on manual scribing tools with digital vernier scales. This presented severe bottlenecks:
* **High Operator Fatigue:** Manual re-calibration to zero was required for every individual unit.
* **Angular Error Risk:** Hand-held alignment caused inconsistent line angles.
* **Low Scalability:** High production volumes caused worker productivity to fluctuate over repetitive shifts.

---

## Mechanical Design & Prototyping Process

The project was executed through an iterative prototyping approach spanning approximately three iterations to systematically identify operational loopholes, evaluate functionality, and refine the design before moving forward:
1. **Pneumatic Clamping Stage:** Utilized dual pneumatic cylinders (Festo DFM-16-20-P-A-GF) mounted on a 17-4PH H900 stainless steel plate (chosen for its high 400 HB hardness) to automatically center and secure the load cell. Tungsten carbide drill bits were used on a pillar drill for precision fabrication.
2. **Spring-Loaded Shock-Absorbing Tool Head:** Direct tool attachment caused tool fracture due to high frictional stress during scribing. Designed a custom spring-loaded shock-absorbing assembly in **SOLIDWORKS** with an M5 knurled nut for angular locking.
3. **Linear Motion:** Integrated a Newmark stepper motor linear guide module limited by PNP proximity sensors to execute precise vertical displacement.

---

## Electronics & Control Architecture

The system integrates low-cost, open-source microcontrollers with robust industrial hardware:
* **Microcontroller:** Arduino Nano
* **Driver IC:** **ULN2803A** Darlington transistor array configured with common free-wheeling diodes to safely drive 24V industrial pneumatic solenoid valves from 5V Arduino digital outputs.
* **Stepper Driver Configuration (Driver & DIP Switches):**
  * **Current Limit:** Set to ~1.0 A RMS (`S4=ON, S5=OFF, S6=ON`) matching the motor rating safely.
  * **Microstepping:** Configured to **8 microsteps (1600 steps/rev)** (`S1=OFF, S2=OFF, S3=ON`) to balance smooth motion and precision.

---

## Firmware Features

The firmware handles real-time execution via an optimized state machine (`IDLE`, `RUNNING`, `EMERGENCY_STOPPED`, `RETURNING_HOME`):
* **Hardware Interrupts (`INT0`, `INT1`):** Instant stop triggers via proximity sensors and emergency buttons.
* **Double-Press Safety Logic:** Implemented an emergency recovery window requiring a validated double-press timeout sequence before homing.

*(View the complete script in the [`/firmware`](./firmware) folder).*

---

## PCB Design & Status

* **Schematic & Layout:** Designed using Eagle CAD, featuring an Arduino Nano socket, TSR-2-2450 voltage regulator, and GND polygon copper pour planes.
* **Status:** Fully prototyped and functionally validated on breadboard/perfboard. However, physical PCB fabrication was ultimately deferred due to scope expansion—incorporating an integrated horizontal scribing mechanism alongside the vertical axis—as well as prototype-stage controller reliability constraints that necessitated further design iteration.

---

## Media & Prototype Showcase

* Check out the working prototype videos and photos
