#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#define BUFFER_SIZE 100

void Init_bluetooth();
void set_sent(int x);
int get_sent();
void EUSCIA2_IRQHandler(void);
int check_bluetooth();

#endif
