# Developer's Guide
## Introduction
This Developer's Guide serves as a introduction and guide for anyone interested in the technical aspects of this project or future developers who may need to extend, optimise, or fix bugs in the codebase. The focus of this project is a configurable lab timer which could run multiple timers simultaneously and provide standard clock functionality.
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
  - Components: **LCD**, **Shift Register LEDs**, **Micro-controller**.
- **CONFIG State**:
  - Components: **LCD**, **Keypad**, **Micro-controller**.

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

### Hardware Components
#### Keypad

