/*
 * timer_config.h
 *
 *  Created on: Jul 26, 2024
 *      Author: Anna Yang
 */

#ifndef SRC_TIMER_CONFIG_H_
#define SRC_TIMER_CONFIG_H_

enum State {
	CONFIGURE_TIMER_COUNT,
	CONFIGURE_TIMER_DURATION,
	CONFIGURE_TIMER_NAME,
	CONFIGURE_TIMER_ALERT
};

typedef struct {
    char key;
    const char *chars;
} KeyMap;

typedef struct {
    uint16_t freq;
    uint16_t duration;
} Note;

typedef struct {
    Note notes[16];
    uint16_t num_notes;
} Song;

typedef struct {
	int duration;
	int remaining_time;
	int running;
	char name[17];
	Song alert;
} Timer;

typedef struct {
	enum State state;
	int num_timers;
	Timer timers[4];
} User;

extern User user;
extern volatile int note_playing;

void welcome();
void choose_timer_count();

void config_specific_timer();
void enter_timer_duration(int timer_index);
void check_timer_duration(int input_secs, int timer_index);
void display_time(int input_secs);

void enter_timer_name(int timer_index);
void t9_typing(int key, char *input_text);

void choose_timer_alert(int timer_index);
void init_alerts();
void play_alert(Song *song);

#endif /* SRC_TIMER_CONFIG_H_ */
