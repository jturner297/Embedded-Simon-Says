# Embedded Simon Says 

<img src="assets/STM-SAYS_thumbnailGIF.gif" width="500" />

A faithful recreation of the classic Simon memory game, built entirely on STM32 using LEDs and buttons.

## Overview
**Embedded Simon Says** recreates the iconic memory game using the **STM32L476RG microcontroller**. It features **two distinct gameplay modes**. Perfect for practicing pattern recognition or just testing your memory.



### Learning Mode 
Designed to help players memorize and study patterns at their own pace. 
  - Players can view the entire pattern from the start
  - Unlimited attempts at recreating the pattern, no penalties for mistakes
  - Goal: master the sequence

### Challenge Mode
Designed to test the player’s memory. 
  - Players must replicate an increasingly complex pattern perfectly
  - One mistakes end the gane
  - Each succesful round turns 2 more LEDs on at the end of the game
  - Goal: complete 8 rounds to light all 16 LEDs on the results screen

## Demo
<table>
  <tr>
    <td>
      <a href="https://www.youtube.com/watch?v=YpWlhrKDB2I">
        <img src="assets/stm-says - demo_thumbail_B.png" width="400" />
      </a>
    </td>
  </tr>
  <tr>
    <td><sub><strong>Gameplay/Features Showcase</strong></sub></td>
  </tr>
</table>

## Features
✅ Two Game Modes:
 - Learning: View full patterns for training
 - Challenge: Authentic Simon experience with growing pattern sequences

🧠 4 unique patterns
 - Each with different levels of complexity  

🔁 Instantly switch between game modes and patterns with the built-in USER button
 - Tapping switches to the next pattern
 - Holding changes active game mode

🕹️ Interrupt-based input handling
 - External interrupts on PA1, PA4, PC0, PC2, and PC13
 - Software debouncing ensures clean button logic

⏱️ Precise timing  
 - TIM2 for animations
 - SysTick for timekeeping
 
💡 Full LED-based visual interface
 - Visual scoreboard in Challenge Mode that tracks player progress
 - Mode transitions and active pattern indicated on four white LEDs
 - Single Green LED used to indicate when system is ready for player input
 - Single Red LED indicates when a player makes a mistake









