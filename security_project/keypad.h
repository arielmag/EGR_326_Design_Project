#ifndef _KEYPADH_
#define _KEYPADH_

#define ENTER_KEY '#'
#define HOME_KEY '*'

char keypad_getkey();
int debounce();
void Init_keypad();
int check_pressed(void);
#endif
