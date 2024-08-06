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

For timer configuration, the `Timer` struct is used to store the details for each timer. It consists of five fields:

- `duration`: Indicates the length of the timer in seconds.
- `remaining_time`: Indicates the remaining time left in the timer in seconds.
- `running`: Indicates whether the timer is currently running or not.
- `name`: Indicates the name of the timer.
- `alert`: Indicates the alert that will be played when the timer has finished.

Similarly, the `User` struct is used to store the timer configuration of the user. It consists of four fields:

- `state`: Indicates the stage of timer configuration that the user is currently at, which can either be `CONFIGURE_TIMER_COUNT`, `CONFIGURE_TIMER_DURATION`, `CONFIGURE_TIMER_NAME`, `CONFIGURE_TIMER_ALERT`.
- `num_timers`: Indicates the number of timers that the user manages.
- `timers`: Stores the timer information obtained from the configuration stage, with fields from above.

Timer alerts are stored in the `Song` struct, which consists of three fields:
- `notes`: Stores the `freq` and `duration` assigned to each note of the alert.
- `num_notes`: Stores the number of notes in the alert.
- `id`: Stores the alert ID.

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

#### ConfigState
```c
typedef enum {
   CONFIGURE_TIMER_COUNT,
   CONFIGURE_TIMER_DURATION,
   CONFIGURE_TIMER_NAME,
   CONFIGURE_TIMER_ALERT
} ConfigState;
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
### User Struct
```c
typedef struct {
	ConfigState state;
	int num_timers;
	Timer timers[4];
} User;
```

### State Navigation
The main method of the user changing states is implemented through the buttons and their respective pins `SW1_Pin`, `SW2_Pin` and `SW3_Pin`, with the following functionalities
- `SW1_Pin` cycles between the main modes
- `SW2_Pin` cycles between the sub modes of that main mode
- `SW3_Pin` cycles between the either `DISPLAY` or `CONFIG` modes

## Timer Mode
### Initial Configuration 
#### Overview
The configuration stage has 4 states: `CONFIGURE_TIMER_COUNT`, `CONFIGURE_TIMER_DURATION`, `CONFIGURE_TIMER_NAME`, and `CONFIGURE_TIMER_ALERT`. It is the initial configuration stage fo the user's timers, allowing them to enter the number of timers they desire, as well as customise their duration, name, and alerts. Refer to [this image](https://github.com/Saitama909/Desn2000-project/blob/1ff061949d9f06a18a13b71e1611b81738e4138e/images/timer%20state%20diagram.jpg) for a visual representation of the state transitions for timer configuration and display.

#### Software Structure
- **Main Functionality Files**: `timer_config.c`
- **State Functions**:
  - `CONFIGURE_TIMER_COUNT`: Handled by `void choose_timer_count()`.
  - `CONFIGURE_TIMER_DURATION`: Handled by `void enter_timer_duration(int timer_index)`.
  - `CONFIGURE_TIMER_NAME`: Handled by `void enter_timer_name(int timer_index)`.
  - `CONFIGURE_TIMER_ALERT`: Handled by `void choose_timer_alert(int timer_index)`.

#### Subsystems
- **CONFIGURE_TIMER_COUNT State**:
  - Components: **LCD**, **Keypad**, **Microcontroller**.
- **CONFIGURE_TIMER_DURATION State**:
  - Components: **LCD**, **Keypad**, **Microcontroller**.
- **CONFIGURE_TIMER_NAME State**:
  - Components: **LCD**, **Keypad**, **Microcontroller**.
- **CONFIGURE_TIMER_ALERT State**:
  - Components: **LCD**, **Keypad**, **Buzzer**, **Microcontroller**.

#### Configuring the Timer
1. **Initial Configuration**:
   - Begins with `void welcome()`, displaying a welcome message on the LCD.
2. **Entering Number of Timers**:
   - Enters `void choose_timer_count()`, allowing the user to input the number of timers they want, through the keypad.
   - Displays the number of timers on the LCD, only allowing one character to be entered.
3. **Validating Number of Timers**:
   - After `#` key is pressed, the same function validates the entered number.
     - If the number is less than 1 or more than 4, or no number is entered, it shows an error message on the LCD.
     - If the number is valid, it sets `num_timers` to the entered value in the `User` struct.
4. **Entering Timer Duration**:
   - Enters `void enter_timer_duration(int timer_index)`, allowing the user to input the duration in the format `HMMSS`.
   - Displays the entered time on the LCD.
5. **Validating Timer Duration**:
   - After entering the duration, the function `void check_timer_duration(int input_secs, int timer_index)` validates the entered time.
     - If the time is less than 30 seconds or more than 1 hour, it shows an error message on the LCD.
     - If the time is valid, it sets `duration` and `remaining_time` to the entered value in seconds.
6. **Entering Timer Name**:
   - Enters `void enter_timer_name(int timer_index)`, allowing the user to input the name of the timer, using T9 typing (similar to old nokia phones) through the `void t9_typing(int key, char *input_text)` function.
     - The `KeyMap` struct stores the `key` and `*chars` of each key on the keypad and the relevant characters it can input.
   - Displays the entered name on the LCD.
7. **Validating Timer Name**:
   - After entering the name, the function `bool check_timer_name(int timer_index, char *input_text)` validates the entered name.
     - If the name is more than 13 characters, less than 1 character, or already exists with another timer, it shows an error message on the LCD.
     - If the name is valid, it sets `name` to the entered value.
8. **Entering Timer Alert**:
   - Enters `void choose_timer_alert(int timer_index)`, allowing the user to choose the alert associated to the timer.
   - Previews the chosen alert with the function `void play_alert(volatile Song *song)` and displays the song number on the LCD.
9. **Validating Timer Alert**:
   - After choosing the timer alert, the function `bool check_timer_alert(int timer_index, int selected_song)` validates the chosen alert.
     - If the timer alert already exists on another timer, it shows an error message on the LCD.
     - If the timer alert is valid, it sets `alert` to the chosen value.
10. **Moving to Display State**:
   - Exits `config_specific_timer()` and transitions to the `EnterTimer()` function.

#### Key Features/Functions
- **Hardware Timers**: Uses timer `htim7` to implement the timer which changes the value `remaining_time` when called every second.
- **PWM/Buzzer**: Uses timer `htim1` to implement pulse width modification to achieve varying frequencies of the buzzer, and timer `htim16` to achieve varying durations of notes from the buzzer, stopping the buzzer every time it is called.

### Timers
#### Overview
The timer mode has two states after initial configuration: `DISPLAY` and `CONFIG`. In `DISPLAY` mode it displays the timer on the bottom row of the LCD in the format ``h:mm:ss`` and can be started, stopped or reset. By default, timer 1 is displayed, and different timers can be displayed by pressing `SW2`. By pressing `A`, `B`, `C`, `D`, it can start timers 1, 2, 3, 4, respectively. In the `CONFIG` mode it prompts the user to enter a timer duration, timer name, and timer alert again, just like the initial configuration stage.

#### Software Structure
- **Main Functionality Files**: `timer.c`
- **State Functions**:
  - `DISPLAY`: Handled by `void EnterTimer()`.
  - `CONFIG`: Handled by `void ReconfigTimer()`.

#### Subsystems
- **DISPLAY State**:
  - Components: **LCD**, **Keypad**, **Microcontroller**.
- **CONFIG State**:
  - Components: **LCD**, **Keypad**, **Microcontroller**.

#### Timer Functionality
#### Displaying the Timer

1. **Initial Display**:
   - Begins with `void EnterTimer()`, clearing the LCD and displaying the timer number and timer name on the first row.
   - The second row shows the remaining time, which updates every second.
2. **Handling Keypad Input**:
   - 'A', 'B', 'C', 'D' keys toggle the timers 1, 2, 3, 4 respectively.
3. **Updating Timer on LCD**:
   - The function `void update_time(int input_secs)` updates the LCD display with the current timer value in `H:MM:SS` format.
4. **Running the Timer**:
   - If the timer is running, it decreases the `time_remaining` field in each timer every second and updates the LCD.
   - When the timer reaches zero, it triggers an alert for the respective timer using `void play_timer_alert(int timer_index)`.

#### Reconfiguring the Timer
This process is extremely similar to the initial configuration of the timer.

1. **Initial Reconfiguration**:
   - Begins with `void ReconfigTimer()`, resetting the timer and displaying the prompt to enter the timer duration.
2. **Entering Timer Duration**:
   - Enters `void enter_timer_duration(int timer_index)`, allowing the user to input the duration in the format `HMMSS`.
   - Displays the entered time on the LCD.
3. **Validating Timer Duration**:
   - After entering the duration, the function `void check_timer_duration(int input_secs, int timer_index)` validates the entered time.
     - If the time is less than 30 seconds or more than 1 hour, it shows an error message on the LCD.
     - If the time is valid, it sets `duration` and `remaining_time` to the entered value in seconds.
4. **Entering Timer Name**:
   - Enters `void enter_timer_name(int timer_index)`, allowing the user to input the name of the timer.
   - Displays the entered name on the LCD.
5. **Validating Timer Name**:
   - After entering the name, the function `bool check_timer_name(int timer_index, char *input_text)` validates the entered name.
     - If the name is more than 13 characters, less than 1 character, or already exists with another timer, it shows an error message on the LCD.
     - If the name is valid, it sets `name` to the entered value.
6. **Entering Timer Alert**:
   - Enters `void choose_timer_alert(int timer_index)`, allowing the user to choose the alert associated to the timer.
   - Previews the chosen alert with the function `void play_alert(volatile Song *song)` and displays the song number on the LCD.
7. **Validating Timer Alert**:
   - After choosing the timer alert, the function `bool check_timer_alert(int timer_index, int selected_song)` validates the chosen alert.
     - If the timer alert already exists on another timer, it shows an error message on the LCD.
     - If the timer alert is valid, it sets `alert` to the chosen value.
8. **Returning to Display State**:
   - Exits `ReconfigTimer()` and transitions to the `EnterTimer()` function.

#### Key Features/Functions

- **Hardware Timer**: Uses timer `htim7` to implement the timer which changes the value `remaining_time` when called every second.

## Standard Clock Modes
### Clock
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

#### Clock Functionality
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

#### Key Features/Functions

- **RTC Timer**: Both `DISPLAY` and `CONFIG` states use the `RTC` Timer to retrieve the current time. It is initialized in `main.c` through the `hrtc` in the `static void MX_RTC_Init(void)` function.
- **Initial Display Time**: When the timer is first switched on, the default display time is set to the value of `hrtc`.
- **Clock Motor**: When the hour changes or when the clcok mode is first entered the motor will move corresponding to the hour this is implemented through `void DisplayDateTime()` and `void Motor(int steps)`

### Alarm
#### Overview
The alarm mode has two states: `DISPLAY` and `CONFIG`. In `DISPLAY` mode it displays the word 'Alarm' on the top row of the LCD and can be enabled, disabled, stopped or snoozed. In the `CONFIG` mode it prompts the user to enter an alarm time by setting the hours, minutes and seconds, in that order.

#### Software Structure
- **Main Functionality File**: `alarm.c`
- **State Functions**:
  - `DISPLAY`: Handled by `void AlarmFunctionality()`.
  - `CONFIG`: Handled by `void GetUserInputAlarm()`.

#### Subsystems
- **DISPLAY State**:
  - Components: **LCD**, **Keypad**, **Microcontroller**.
- **CONFIG State**:
  - Components: **LCD**, **Keypad**, **Microcontroller**.

#### Alarm Functionality
#### Configuring the Alarm
1. **Initial Configuration**:
   - Begins with `void GetUserAlarmInput()`, displaying a prompt to configure the alarm.
2. **Entering Alarm Details**:
   - Still in `void GetUserAlarmInput()`, allowing the user to input the hours, minutes and seconds in that order.
   - This function then calls `void SetAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds)`, which sets the RTC to trigger an alarm when that certain time occurs.
3. **Returning to Display State**:
   - Exits `void GetUserAlarmInput()` and transitions to the `void DisplayAlarm()` function.

#### Displaying the Alarm
1. **Initial Display**:
   - Begins with `void DisplayAlarm()`, clearing the LCD and displaying "Alarm" on the first row.
2. **Handling Keypad Input**:
   - 'A' key enables an alarm that has not been completed and is currently disabled.
   - 'B' key disables an alarm that has not been completed.
   - 'C' key stops the alarm while it is triggered.
   - 'D' key snoozes the alarm while it is triggered.
4. **Completeting the Alarm**:
   - Once the set time occurs, an alarm gets triggered and a buzzer goes off and an LED lights up.

#### Key Features/Functions
- **RTC Timer**: The `CONFIG` state use the `RTC` Timer to retrieve the current alarm. It is initialized in `main.c` through the `hrtc` in the `static void MX_RTC_Init(void)` function, and is then set to the new time in `void SetAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds)`. The SetAlarm function calls `HAL_RTC_GetAlarm`, and then edits the hours, minutes and seconds and sets the alarm with `HAL_RTC_SetAlarm_IT`.
The `DISPLAY` state uses the internal alarm A, triggering `void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)`, in `alarm.c`, when the specific time occurs.

### Standard Clock Timer
#### Overview

The standard clock timer mode has two states: `DISPLAY` and `CONFIG`. In `DISPLAY` mode it displays the timer on the bottom row of the LCD in the format ``hh:mm:ss`` and can be started, stopped or reset. In the `CONFIG` mode it prompts the user to enter a timer duration on the bottom row of the LCD in the format ``hh:mm:ss`` 

#### Software Structure

- **Main Functionality File**: `stdtimer.c`
- **State Functions**:
  - `DISPLAY`: Handled by `void DisplayTimer()`.
  - `CONFIG`: Handled by `void ConfigTimer()`.

#### Subsystems

- **DISPLAY State**:
  - Components: **LCD**, **Keypad**, **Microcontroller**.
- **CONFIG State**:
  - Components: **LCD**, **Keypad**, **Microcontroller**.

#### Standard Clock Timer Functionality
#### Configuring the Timer

1. **Initial Configuration**:
   - Begins with `void ConfigTimer()`, resetting the timer and displaying the prompt to enter the timer duration.
2. **Entering Timer Duration**:
   - Enters `int EnterTimerDuration()`, allowing the user to input the duration in the format `HHMMSS`.
   - Displays the entered time on the LCD.
3. **Validating Timer Duration**:
   - After entering the duration, the function `int CheckTimerDuration(int input_secs)` validates the entered time.
     - If the time is less than 30 seconds or more than 1 hour, it shows an error message on the LCD.
     - If the time is valid, it sets `duration` and `time_left` to the entered value in seconds.
4. **Returning to Display State**:
   - Exits `void ConfigTimer()` and transitions to the `void DisplayTimer()` function.

#### Displaying the Timer

1. **Initial Display**:
   - Begins with `void DisplayTimer()`, clearing the LCD and displaying "Standard Timer:" on the first row.
   - The second row shows the remaining time, which updates every second.
2. **Handling Keypad Input**:
   - '#' key toggles the timer between running and stopped states.
   - '*' key resets the timer to the initial duration.
3. **Updating Timer on LCD**:
   - The function `void updateTimerLCD(uint32_t count)` updates the LCD display with the current timer value in `HH:MM:SS` format.
4. **Running the Timer**:
   - If the timer is running, it decreases `time_left` every second and updates the LCD.
   - When the timer reaches zero, it triggers an alert using `void stdTimerAlert()`.

#### Key Features/Functions

- **Timer**: Uses timer `htim15` to implement the timer which changes the value `time_left` when called
- **Alert**: Uses `stdTimerAlert()` function to play alert which utilises `htim16`
  
### Stopwatch

#### Overview

The stopwatch mode has only one state: `DISPLAY`. It displays the elapsed time in the format `hours:minutes:seconds` on the LCD. The stopwatch can be started, stopped, and reset using keypad inputs.

#### Software Structure

- **Main Functionality File**: `stopwatch.c`
- **State Function**:
  - `DISPLAY`: Handled by `void EnterStopwatch()`.

#### Subsystems

- **DISPLAY State**:
  - Components: **LCD**, **Keypad**, **Microcontroller**.

#### Stopwatch Functionality
#### Displaying the Stopwatch

1. **Initial Configuration**:
   - Begins with `void EnterStopwatch()`, resetting the LCD and initializing the stopwatch variables.
2. **Displaying Time**:
   - Continuously updates the LCD to display the elapsed time in the format `hours:minutes:seconds`.
   - Uses a timer interrupt to keep track of milliseconds.
3. **Keypad Controls**:
   - '#': Toggles the stopwatch between running and stopped states.
   - '*': Resets the stopwatch to zero.
4. **Exiting Display State**:
   - Checks for a change in the device state to exit `void EnterStopwatch()` and return to the main state handling function.

#### Key Features/Functions

#### `void EnterStopwatch()`
Initializes the LCD and enters a loop to handle keypad input and update the LCD display.

#### `void LCD_Reset()`
Clears the LCD and sets the cursor to the home position.

#### `void updateLCD(uint32_t count)`
Formats the elapsed time and updates the LCD display if the seconds have changed.

#### `void toggleStopwatch()`
Starts or stops the timer interrupt to control the stopwatch running state.

#### `void resetStopwatch()`
Stops the timer interrupt and resets the elapsed time to zero.

- **Stopwatch Timer** : Uses `htim6` to start and stop the stopwatch

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

Note: LDR R32 is set to `ADC2_IN5` an anlogue to digital pin, LED D3 and LED D2 are set to `TIM3` channels 1 and 2 respectively. 
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

### Power Supply

Needs a wall socket to provide the timer with power




