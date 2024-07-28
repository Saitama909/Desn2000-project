## State Definition 

Uses struct `DeviceState` to store the state of the timer with 4 fields `mainMode`, `timerMode`, `clockMode`, `modeState` of type enum `MainMode`, `TimerMode`, `ClockMode` and `ModeState` respectively

The `mainMode` field corresponds to the two main functionalities, with each main modes defined as 
- `TIMER_MODE`,
- `CLOCK_MODE`
  
of type enum `MainMode`

The `timerMode` field corresponds to the main subfunctions when `mainMode` is in `TIMER_MODE` (the timer mode), namely the four timers.

The `clockMode` field correspond to the main subfunctions when `mainMode` is in `CLOCK_MODE` (the standard clock mode), namely the clcok, alarm, countdown timer and stopwatch which are defined as `CLOCK` `ALARM`, `COUNTDOWN` and `STOPWATCH` of type enum `ClockMode` respectively.
 
The last field `modeState` corresponds to whether the subfunction is   in the displaying or configuring state, defined as `DISPLAY` and `CONIFG` of type enum `ModeState` respectively.

## Standard Clock modes
### Clock
Functionality Overview:
A standard clock that displays the time and is configurable. It's format is in `day/month/year weekday` on the top row of the LCD and `hour:minute:second am/pm` on the bottom row of the LCD

Software Documentation:
Most of the code responsible for clock functionality is in the `clock.c` file, with its `DISPLAY` and `CONFIG` states corresponding to the `DisplayClock` and `ConfigClock` functions respectively. 

Clock Subsystem and function/s:
The `DISPLAY` state subsystem is comprised of the **LCD**, **Shift Register LEDs** and the **Micro-controller**

The `CONFIG` state subsystem is comprised of the **LCD**, **Keypad** and the **Micro-controller**

- Initially starts configuration through `void ConfigClock()` at the current time
- Enters `void EditTime()` and selects the day as the first component to configure, with the selected component indicated by the flashing of LCD of that component (`void Flash()`)
- Uses keypad to configure with the key presses being
  - 'A' to increment the selection
  - 'B' to decrement the selection
  - 'C' to move the selection back an element (e.g. go from month to day)
  - 'D' to move the selection to the next element (e.g. go from day to month)
- Upon pressing moving the selection the next element whilst at the last element (configuring the weekday) the new time will be set as the current time unless it is an invalid time 
  - If the new time is valid the current time will be updated indicated by the LCD message `Time Changed!`
  - If the new time is invalid the current time will not be updated indicated by the LCD message `Invalid Date!`
- The `void ConfigClock()` will be exited and return to the `CheckDeviceState()` function in `main.c` and `deviceState.modeState` will be updated to `DISPLAY` with the timer will enter the display state.


Key features:
For the clock functionality both `DISPLAY` and `CONFIG` states utilise the `RTC` Timer, to retrieve the current time initialised in `main.c` through the `hrtc` in the `static void MX_RTC_Init(void)` function. When the timer is first switched on the default display time will be this value of `hrtc`
