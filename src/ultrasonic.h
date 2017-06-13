#ifndef _ULTRASONIC_H
#define _ULTRASONIC_H

#define ECHO 4	//input
#define TRIG 5	//output

void *ultrasonicDetection(void * param);
void usInit();
int getCM();
void usTerm();

#endif
