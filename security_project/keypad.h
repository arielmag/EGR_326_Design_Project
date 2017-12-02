#ifndef _KEYPADH_
#define _KEYPADH_

#define BACK_KEY '#'
#define HOME_KEY '*'

char keypad_getkey();
int debounce();
void Init_keypad();
char check_pressed(void);
void set_timeout(int value);
int get_timeout();

#endif
