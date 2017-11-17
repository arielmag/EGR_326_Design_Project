#ifndef SENSORS_H_
#define SENSORS_H_

#define OPEN 1
#define CLOSED 0
void PORT2_IRQHandler(void);
void Init_PIR();
int check_PIR();
void Init_hall();

#endif
