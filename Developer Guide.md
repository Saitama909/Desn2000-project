# Developer's Guide
## Introduction
This Developer's Guide serves as a introduction and guide for anyone interested in the technical aspects of this project or future developers who may need to extend, optimise, or fix bugs in the codebase. The focus of this project is a configurable lab timer which could run multiple timers simultaneously and provide standard clock functionality.

## States
### State Definition

The `DeviceState` struct is used to store the state of the timer. It consists of four fields:

- `mainMode`: Indicates the primary mode, which can be either `TIMER_MODE` or `CLOCK_MODE`. This is of type `MainMode`.
- `timerMode`: Indicates the subfunctions available in `TIMER_MODE`. This is of type `TimerMode`.
- `clockMode`: Indicates the subfunctions available in `CLOCK_MODE`. This is of type `ClockMode`. The available modes are `CLOCK`, `ALARM`, `COUNTDOWN`, and `STOPWATCH`.
- `modeState`: Indicates whether the subfunction is in the displaying or configuring state. This is of type `ModeState` and can be either `DISPLAY` or `CONFIG`.

### Enumerations

#### MainMode
```c
typedef enum {
    TIMER_MODE,
    CLOCK_MODE
} MainMode;
```

#### TimerMode
```c
typedef enum {
    TIM1,
    TIM2,
    TIM3,
    TIM4   
} TimerMode;
```

#### ClockMode
```c
typedef enum {
    CLOCK,
    ALARM,
    COUNTDOWN,
    STOPWATCH
} ClockMode;
```

#### ModeState
```c
typedef enum {
    DISPLAY,
    CONFIG
} ModeState;
```

### DeviceState Struct
```c
typedef struct {
    MainMode mainMode;
    TimerMode timerMode;
    ClockMode clockMode;
    ModeState modeState;
} DeviceState;
```
### State Navigation
The main method of the user changing states is implemented through the buttons and their respecive pins `SW1_Pin`, `SW2_Pin` and `SW3_Pin`, with the following functionalities
- `SW1_Pin` cycles between the main modes
- `SW2_Pin` cycles between the sub modes of that main mode
- `SW3_Pin` cycles between the either `DISPLAY` or `CONFIG` modes
- 
### Standard Clock Modes

#### Overview

The clock mode has two states: `DISPLAY` and `CONFIG`. It displays the time in the format `day/month/year weekday` on the top row of the LCD and `hour:minute:second am/pm` on the bottom row.

#### Software Structure

- **Main Functionality File**: `clock.c`
- **State Functions**:
  - `DISPLAY`: Handled by `void DisplayClock()`.
  - `CONFIG`: Handled by `void ConfigClock()`.

#### Subsystems

- **DISPLAY State**:
  - Components: **LCD**, **Shift Register LEDs**, **Microcontroller**.
- **CONFIG State**:
  - Components: **LCD**, **Keypad**, **Microcontroller**.

### Clock Functionality
#### Displaying the Clock
1.  **Initial Configuration**:
   - Begins with `void DisplayClock()`, and resets the LCD and sets the `initial` flag to 1 to display time initially
2. **Displaying Time**:
   - Enters `void DisplayDateTime()`, displaying the current time in the format `day/month/year weekday` on the top row of the LCD and `hour:minute:second am/pm` on the bottom row.
   - Shift Register LED's D20 to D9 will light up cumalatively corresponding to the hour
   - This will continue until the state of the timer changes
3. **Exiting Display State**:
   - Changing the state of the timer will exit `void DisplayClock()` and exit Display State to the next state
#### Configuring the Clock

1. **Initial Configuration**: 
   - Begins with `void ConfigClock()`, starting at the current time.
2. **Editing Time**:
   - Enters `void EditTime()`, selecting the day as the first component to configure.
   - Selected component flashes on the LCD (`void Flash()`).
3. **Keypad Controls**:
   - 'A': Increment the selection.
   - 'B': Decrement the selection.
   - 'C': Move back an element (e.g., month to day).
   - 'D': Move forward an element (e.g., day to month).
4. **Setting the Time**:
   - After configuring the last element (weekday), the new time is validated.
     - If valid, the current time is updated, and the LCD shows `Time Changed!`.
     - If invalid, the current time is not updated, and the LCD shows `Invalid Date!`.
5. **Returning to Display State**:
   - Exits `ConfigClock()`, returns to `CheckDeviceState()` in `main.c`, and updates `deviceState.modeState` to `DISPLAY`.

#### Key Features

- **RTC Timer**: Both `DISPLAY` and `CONFIG` states use the `RTC` Timer to retrieve the current time. It is initialized in `main.c` through the `hrtc` in the `static void MX_RTC_Init(void)` function.
- **Initial Display Time**: When the timer is first switched on, the default display time is set to the value of `hrtc`.

## Hardware Components
### Microcontroller
**Pin Wirings**
All pin wirings follow this document:
https://github.com/Saitama909/Desn2000-project/blob/main/Board%20Pin%20Wiring%20Documentation.pdf
Naming of the corresponding pins for each I/O Device are shown below:

| I/O Device                  | Pin Name    |
|-----------------------------|-------------|
| Button SW1                  | SW1_Pin     |
| Button SW2                  | SW2_Pin     |
| Button SW3                  | SW3_Pin     |
| Shift Register RCLK (latch) | RCLK_Pin    |
| Shift Register SRCLK (serial clock) | SRCLK_Pin |
| Shift Register SER (data in) | SER_Pin     |
| ROW1                        | R1_PIN      |
| ROW2                        | R2_PIN      |
| ROW3                        | R3_PIN      |
| ROW4                        | R4_PIN      |
| COL1                        | C1_PIN      |
| COL2                        | C2_PIN      |
| COL3                        | C3_PIN      |
| COL4                        | C4_PIN      |
| LCD Data D4                 | LCD_D4_PIN  |
| LCD Data D5                 | LCD_D5_PIN  |
| LCD Data D6                 | LCD_D6_PIN  |
| LCD Data D7                 | LCD_D7_PIN  |
| LCD Control E               | LCD_E_PIN   |
| LCD Control RW              | LCD_RW_PIN  |
| LCD Control RS              | LCD_RS_PIN  |
| COIL A                      | COIL_A_Pin  |
| COIL B                      | COIL_B_Pin  |
| COIL C                      | COIL_C_Pin  |
| COIL D                      | COIL_D_Pin  |
| LED LD2                     | LD2_Pin     |


### Keypad
A 4x4 Keypad with the 4 row pins configured as inputs and the 4 column pins configured as outputs connected to the **Microcontroller Board** , with related functions located in the `lcd_keypad.c` file.

**Keypad Input**
Implemented through setting each of the column pins to a high state and checking which row pin is now in a high state

### LCD

2 row 16 column, blue background LCD connected to the **Microcontroller**, with related functions located in the `lcd_keypad.c` file.  **HD44780** LCD where more information can be found on its datasheet.

**LCD Display**

Requires initialisation before use through the `void init_magic()` function in `lcd_keypad.c` file prior to displaying anything on the screen.

### Motor

Uses half-step movement to rotate, connected to the coil pins A-D, mainly implemented through the `void Motor` function in `main.c`

### Microcontroller

STM32F303RE ARM Microcontroller, controls the other hardware components through its connection, more information on its datasheet and board schematic (https://github.com/Saitama909/Desn2000-project/blob/main/STM32f303re%20DataSheet.pdf and https://github.com/Saitama909/Desn2000-project/blob/main/coaST-schematic.pdf)

### LEDs

4 normal LED's next to each of the buttons SW1-SW4 and shift register LED's that utilise the Shift Register RCLK(latch),  Shift Register SRCLK (serial clock) and Shift Register SER (data in) for data input and lighting up. 

### Light Dependent Resistors

Of the two only `LDR R32` is used in this project and soley in `void LightBrightness()` in `main.c` to determine the brightness of the LED's based on the environmental lighting






