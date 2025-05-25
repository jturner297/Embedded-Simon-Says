# Embedded Simon Says 

<img src="assets/STM-SAYS_thumbnailGIF.gif" width="500" />

A pattern-matching memory game built on STM32

## Overview
**Embedded Simon Says** is a recreation of the classic game of Simon Says, but built on **STM32** using physical LEDs and buttons. Players must memorize and replicate increasingly complex patterns using the corresponding buttons. The difficulty can be changed anytime using the board's built-in button.

## Demo
<table>
  <tr>
    <td>
      <a href="https://www.youtube.com/watch?v=8bnj9skPk7E">
        <img src="assets/STM-SAYS_demo_thumbnail.jpg" width="400" />
      </a>
    </td>

  </tr>
  <tr>
    <td><sub><strong>Gameplay/Features Showcase</strong></sub></td>
  </tr>
</table>

## Features
🎮 **Player Interaction**
  - Players use four color-coded buttons that correspond to LED blocks to enter the pattern
  - 

🔄 **Four Unique Patterns/Sequences**


🔧 **Modular Codebase**
  - Organized across multiple files like input.c, patterns.c, leds.c for clarity and scalability
  - Allows users to easily add or modify gameplay patterns by updating a single module, without affecting core game logic




⚙️ **State machine-driven architecture**
  - Clearly defined game phases: serve, movement, hitzone, miss, win animation
  - Transitions managed through timers and input state

⚡ **Hit detection and reaction feedback**
  - HITZONE LEDs toggle off briefly when a button is pressed
  - Timed LED feedback provides clear input acknowledgment

🕒 **Real-time processing using timers**
  - TIM2 used to control LED movement speed and animation
  - SysTick used for millisecond timekeeping and debouncing

👆 **Interrupt-based input handling**
  - External interrupts on PA1, PA4, and PC13
  - Software debouncing ensures clean button logic

🧠 **Scoring and win logic**
  - Score resets on a miss
  - First player to win 3 consecutive rounds wins the game
  - Flashing POINTS LEDs indicate game win/victory

🔁 **Instant Mode switching**
  - Pressing the built-in USER button toggles between MOVE_MODE and PLAY_MODE

🧪 **Low-level embedded C**
  - Written without HAL; uses direct register manipulation (CMSIS)
