# User Guide
## Table of Contents
- [Introduction](#introduction)
- [Device Overview](#device-overview)
- [Operating Instructions](#operating-instructions)
  - [Standard Operations](#standard-operations)
  - [Advanced Features](#advanced-features)


## Introduction
Welcome to the User Guide for the LabSync Customisable Laboratory Timer. This guide is designed to help you make the most of your new timer, whether you're setting up or operating the device. Our laboratory timer is a precise and reliable tool crafted to meet the rigorous demands of scientific research and laboratory work. 
The focus of the LabSync timer pod is creating a configurable lab timer which could run multiple timers simultaneously and provide standard clock functionality.

### How to use this User Guide
This user guide is organised to provide clear and concise information on the LabSync's features and operations. It is structured as follows:

Device Overview: An introduction to the timer’s components and features.<br>
Operating Instructions: Detailed instructions on how to use the timer.

We hope you find this guide helpful. If you have any questions or need further assistance, please refer to the Support and Contact Information section at the end of this document.

## Device Overview
<img src="images/PodDiagram.png" alt="LabSync diagram" width="600" height="400">

Diagram of LabSync

<img src="images/Initial.jpg" alt="Initial set up" width="600" height="400">

Initial set up of LabSync

#### Important components highlighted on the product:
- **5V Power Supply**
- **USB Connection**
- **Reset Button**
- **Switch 1**
- **Switch 2**
- **Switch 3**
- **Keypad**
- **LDR Sensors**
- **LCD Screen**
- **LED Bar**
- **Buzzer**

The LabSync contains a simple and easy-to-use user interface, which creates a seamless experience for the user.

The LabSync is set up with two main modes:
- Standard Clock Mode
- Custom Timer Mode

To switch between these two modes, simply use Switch 1, labelled as **SW1** on the board.

Each mode has its own unique features:
- **Standard Clock Mode**
  - Clock
  - Alarm
  - Standard Timer
  - Stopwatch

- **Custom Timer Mode**
  - Timer 1
  - Timer 2
  - Timer 3
  - Timer 4

To switch between any of the features of either mode, simply use Switch 2, labelled as **SW2** on the board.

Once you have selected the feature you want to use, some features may need configuration to your specific needs.

To do so, simply use Switch 3, labelled as **SW3** on the board, to switch from display mode to configuration mode or vice versa.

**Overview**
- **SW1: Switch 1** &rarr; Allows switching between the two main modes
- **SW2: Switch 2** &rarr; Allows switching between submodes/features
- **SW3: Switch 3** &rarr; Allows switching between configuration mode and display mode

## Operating Instructions
### Standard Operations
This section provides instructions on how to use the LabSync's standard operations.

#### Getting Started
**SW1** &rarr; Allows switching between the standard clock mode and the customisable laboratory timer mode.<br>
**SW2** &rarr; Allows switching between each sub mode when you're in either of the main modes.

If you're in the standard clock mode, you can switch between:
- [Clock](#clock)
- [Alarm](#alarm)
- [Standard timer](#standard-timer)
- [Stopwatch](#stopwatch)

If you're in the custom timer mode, you can switch between:
- Timer 1
- Timer 2
- Timer 3
- Timer 4

**SW3** &rarr; Allows switching between configuration modes and display modes.

#### Clock
This feature displays the current date and time.

<img src="images/Clock.jpg" alt="Clock diagram" width="600" height="400">

##### ***Configuring the clock***
After connecting your device and setting it up correctly, to configure the clock:
- If not already in the standard clock mode, press **SW1** once to switch to the standard clock mode
- Press **SW3** once to switch to the configuration mode

Once you're in configuration mode, using the keypad:
- **'A'** &rarr; increments the selection
- **'B'** &rarr; decrements the selection
- **'C'** &rarr; moves back an element (e.g from month to day)
- **'D'** &rarr; moves forward an element (e.g from day to month)

  The order of elements is:
  1. Day
  2. Month
  3. Year
  4. Seconds
  5. Minutes
  6. Hours
  7. Day of the week

Pressing **'D'** when you are on the last element checks if all the data entered is valid and then sets up the clock, time and date if valid. If not valid, the current time and date do not get updated.

##### ***Displaying the clock***
Once the entered time and date are validated, the clock switches from configuration mode to display mode automatically. In this mode, you should see the date and current time displayed on the screen.

To edit the clock's date and time again, simply press **SW3** to enter configuration mode.

#### Alarm
This feature sets an alarm for some time in the future, and plays a sound and lights up an LED when the alarm is triggered.

<img src="images/AlarmConfig.jpg" alt="Alarm config diagram" width="600" height="400">
<img src="images/AlarmDisplay.jpg" alt="Alarm display diagram" width="600" height="400">


##### ***Configuring the alarm***
After connecting your device and setting it up correctly, to configure the clock
- If not already in the standard clock mode, press **SW1** to switch to the standard clock mode
- Press **SW2** once to change from clock mode to alarm mode
- Press **SW3** once to switch to the configuration mode

Once you're in configuration mode, you will be asked to enter hours, minutes and seconds in that order. To move from one element to the next, press the **'#'** on the keypad. 

##### ***Displaying the alarm***
Once the last element is entered and the '#' key is pressed, the alarm switches automatically from configuration mode to display mode. In this mode, you should see 'Alarm' on the LCD, and the display mode contains four features:
1. **Enable an existing disabled alarm** &rarr; Press **'A'** on the keypad
2. **Disable an existing alarm** &rarr; Press **'B'** on the keypad
3. **Stop the alarm** &rarr; Press **'C'** on the keypad
4. **Snooze the alarm** &rarr; Press **'D'** on the keypad

*Note: These features can only be used in the Alarm display mode.

Once the alarm is complete, you will be notified by a buzzer and an LED lighting up on the board for a short period of time. The buzzer and LED turns off automatically after some time.

#### Standard Timer
This feature counts down from a certain time to 00:00:00.

<img src="images/StdTimerConfig.jpg" alt="Std timer config diagram" width="600" height="400">
<img src="images/StdTimerDisplay.jpg" alt="Std display config diagram" width="600" height="400">

##### ***Configuring the standard timer***
After connecting your device and setting it up correctly, to configure the standard timer
- If not already in the standard clock mode, press **SW1** to switch to the standard clock mode
- Press **SW2** twice to change from clock mode to standard clock timer mode
- Press **SW3** once to switch to the configuration mode

Once you're in configuration mode:
- Use the numbers on the keypad to enter your input
- Input is read in an **HHMMSS** format (H - hours, M - minutes, S - seconds)
- Once the input is entered, it is also displayed on the LCD

*Note: Timer cannot be shorter than 30 seconds and longer than 1 hour.

##### ***Displaying the standard timer***
To move from the configuration mode to the display mode, press the **'#'** key on the keypad.

In the display mode:
- **'#'** &rarr; Starts and stops the timer
- **'*'** &rarr; Resets the timer to the configured time

To change the timer, switch to the configuration mode by pressing **SW3** again.

#### Stopwatch 
This feature counts up from 00:00:00.

<img src="images/Stopwatch.jpg" alt="Stopwatch diagram" width="600" height="400">

##### ***Configuring and Displaying the timer***
After connecting your device and setting it up correctly, to use the stopwatch feature
- If not already in the standard clock mode, press **SW1** to switch to the standard clock mode
- Press **SW2** three times to change from clock mode to stopwatch mode

Once you're in the stopwatch mode:
- **'#'** &rarr; Starts and stops the timer
- **'*'** &rarr; Resets stopwatch to 00:00:00

### Advanced Features
#### Custom Timers
This feature allows you to customise and use upto four timers, either at the same time or sequentially.

<img src="images/CusTimerConfig.jpg" alt="Custom Timer Config diagram" width="600" height="400">
<img src="images/CusTimerDisplay.jpg" alt="Custom Timer Display diagram" width="600" height="400">


##### ***Configuring the timers***
After connecting your device and setting it up correctly, to use the custom timers feature:
- If not already in the custom timers mode, press **SW1** to switch to the custom timers mode

Once you're in the custom timers mode:
- You will be asked to input the number of timers you want to use

*Note: This number has to be between 1 and 4

- Use the keypad to enter the number of timers wanted
- Use the **'#'** key on the keypad to enter and validate the input

Once the number of timers has been selected:
- Use the numbers on the keypad to enter your input for how long each timer should be
- Input is read in an **HHMMSS** format (H - hours, M - minutes, S - seconds)
- Once the input is entered, it is validated and is then displayed on the LCD

*Note: Timer cannot be shorter than 30 seconds and longer than 1 hour.

Once the timer has been set:
- You are prompted to enter a name for each timer, using T9 typing on the keypad
- Once the input is entered, it is validated and then displayed on the LCD

*Note: Name cannot be shorter than 1 character, longer than 30 characters or already exist with another timer.

Once each timer has a name:
- You can select a unique timer alert for each timer
- Each alert can be previewed and then selected

*Note: Each alert can only be used for one timer, i.e. two timers cannot have the same alert.

##### ***Displaying the timers***
Once the timers have been initially configured, we switch to display mode automatically.

Initially, the LCD screen should:
- Display the timer number and name on the first row
- Display the remaining time left on the second row

By default, the first timer is displayed. To move from one timer to the next, use **SW2**.

To start each timer:
- **A** &rarr; Timer 1
- **B** &rarr; Timer 2
- **C** &rarr; Timer 3
- **D** &rarr; Timer 4

When the timer reaches the end, the chosen alert is played.

To reconfigure the timers, switch to the configuration mode by pressing **SW3** again.