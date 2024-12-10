# STM32H7 Microcontroller Learning Workspace

Welcome to my repository! This is a dedicated workspace for learning and experimenting with the STM32H754ZI Nucleo board. Below are the details of the projects I’ve worked on so far, each focusing on a specific aspect of microcontroller programming.

---

## Table of Contents
1. [Overview](#overview)
2. [Projects](#projects)
   - [Project 1: Onboard LED Control](#project-1-onboard-led-control)
   - [Project 2: WS2812B LED Control](#project-2-ws2812b-led-control)
3. [Getting Started](#getting-started)
4. [Notes and Tips](#notes-and-tips)

---

## Overview

This repository serves as my learning ground for mastering STM32H7 microcontroller programming. The projects are structured to progressively introduce key concepts, starting with basic GPIO control and advancing to more complex peripherals like timers and DMA.

---

## Projects

### Project 1: Onboard LED Control

**Objective:**  
Control the onboard LEDs with animations and turn them off with a button press.

**Key Learnings:**  
- Configured GPIO pins to control the onboard LEDs.
- Learned how to create LED animations by adjusting timing.
- Configured GPIO to detect button presses and toggle LED states.

### Project 2: WS2812B LED Control

**Objective:**  
Drive a WS2812B LED to light up the first LED in blue using PWM and DMA.

**Key Learnings:**  
- Configured Timer 3, Channel 2 to generate PWM signals as per the WS2812B datasheet requirements.
- Utilized DMA (Memory-to-Peripheral) for efficient data transfer.
- Verified the timing to ensure compatibility with WS2812B protocol.

**Behavior:**  
- Upon flashing, the first LED lights up **Blue**.
- If the LED does not turn on, press the reset button on the board.

---

## Getting Started

1. **Clone the Repository**  
   ```bash
   git clone https://github.com/yourusername/your-repo-name.git
   cd your-repo-name
   ```

2. **Prerequisites:**  
   - STM32CubeIDE installed
   - STM32H754ZI Nucleo board
   - WS2812B LEDs (for Project 2)

3. **Flashing Instructions:**  
   - Open the respective project folder in STM32CubeIDE.
   - Build and flash the project onto your STM32H7 Nucleo board.
   - For Project 2, ensure the LEDs are properly connected to the board.

---

## Notes and Tips

- **Project 2 Troubleshooting:** If the WS2812B LED does not turn on after flashing, press the **Reset** button on the STM32H754ZI board.
- This repository is a work in progress. Stay tuned for more projects as I delve deeper into STM32 programming!


