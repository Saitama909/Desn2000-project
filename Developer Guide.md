## State Definition 

Uses struct `DeviceState` to store the state of the timer with 4 fields `mainMode`, `timerMode`, `clockMode`, `modeState` of type enum `MainMode`, `TimerMode`, `ClockMode` and `ModeState` respectively

The `mainMode` field corresponds to the two main functionalities, with each main modes defined as 
- `TIMER_MODE`,
- `CLOCK_MODE`
  
of type enum `MainMode`

The `timerMode` field corresponds to the main subfunctions when `mainMode` is in `TIMER_MODE` (the timer mode), namely the four timers.

The `clockMode` field correspond to the main subfunctions when `mainMode` is in `CLOCK_MODE` (the standard clock mode), namely the clcok, alarm, countdown timer and stopwatch which are defined as `CLOCK` `ALARM`, `COUNTDOWN` and `STOPWATCH` of type enum `ClockMode` respectively.

The last field `modeState` corresponds to whether the subfunction is in the displaying or configuring state, defined as `DISPLAY` and `CONIFG` of type enum `ModeState` respectively.