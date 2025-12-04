<div align="center">

<img src="media/WizardCrabLogo.png" alt="SILVER2 Mascot" width="180"/>

# SILVER2 - Stonefish Simulation

**Fast and lightweight simulation of the SILVER2 robot using Stonefish.**

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![ROS 2](https://img.shields.io/badge/ROS_2-Jazzy-22314E.svg)](https://docs.ros.org/en/jazzy/)
[![Stonefish](https://img.shields.io/badge/Simulator-Stonefish-orange.svg)](https://stonefish.readthedocs.io/)

</div>

---

## 🌊 Overview

This repository contains the **Stonefish** simulation environment for the **SILVER2** seabed exploration robot. 

Stonefish is a lightweight, C++ based simulator designed specifically for underwater robotics. It provides a stable and fast physics environment ideal for testing high-level control strategies and acoustic sensors without the overhead of heavier graphics engines.

![Silver2 Stonefish](media/silver2_stonefish.png)

---
## 🚀 Key Features

* **High Performance:** Lightweight physics engine capable of running faster than real-time on modest hardware.
* **ROS 2 Compatibility:** Sensors and actuators are integrated with ROS 2 Jazzy for seamless control and data bridging.
* **Hydrodynamics:** Built-in underwater physics handling buoyancy and drag for rigid bodies.

---

## 🎥 Gallery

### Gait Controller Simulation
Demonstration of the robot's locomotion controller running within the Stonefish environment.

![Silver2 Stonefish Animation](media/silver2_stonefish.gif)

---

## 🛠️ Installation & Usage

### 1. Prerequisites
Ensure your system has the following dependencies:
* [ROS 2 Jazzy Desktop](https://docs.ros.org/en/jazzy/Installation.html)
* [Stonefish Simulator v1.5](https://stonefish.readthedocs.io/)
* A configured Python virtual environment (`venv`) with required libraries (see [`requirements.txt`](requirements.txt)).

### 2. Launch the Simulation
This sets up the ROS 2 workspace containing the simulator's ROS wrapper and the custom robot package.

```bash
# Navigate to your cloned repository
cd ~/PathToWorkspace/silver2_stonefish

# Source ROS 2 and the workspace
source /opt/ros/jazzy/setup.bash
source install/setup.bash

# Activate Python virtual environment
source venv/bin/activate

# Launch the simulation
ros2 launch stonefish_silver silver_simulation.launch.py
```

### 3. Control the Robot
To drive the robot, open a **second terminal** to run the teleoperation node.

```bash
# Source ROS 2
source /opt/ros/jazzy/setup.bash

# Run the keyboard teleop node
ros2 run teleop_twist_keyboard teleop_twist_keyboard
```
Use keys ```i```, ```j```, ```k```, ```l``` to move the robot.

---

## 🦀 The SILVER2 Project

SILVER2 is a bio-inspired robot designed for low-impact seabed interaction. This Stonefish environment is part of a larger ecosystem of simulation tools.

**Check out our other simulators:**
* **Isaac Sim Repo:** [Joagai23/silver2_isaacsim](https://github.com/Joagai23/silver2_isaacsim)
* **Gazebo Repo:** [Joagai23/silver2_gz](https://github.com/Joagai23/silver2_gz)

### Acknowledgements
The project is carried out within the framework of the activities of the Spanish Government through the “Severo Ochoa Centre of Excellence” granted to ICM-CSIC (CEX2024-001494-S) and the Research Unit Tecnoterra (ICM-CSIC/UPC).

This project is supported by the Horizon Europe [**MERLIN Project**](https://merlin-project.org/) [grant number GAP-01189796] and [**Blue Project**](https://www.blue-project.eu/) [grant number 101061354].

<div align="center">


<img src="media/WizardCrabLogo.png" width="80"/>

<sub><i>"Magic happens at the bottom of the sea."</i></sub> </div>