#ifndef _ULTRASONIC_H
#define _ULTRASONIC_H

#define ECHO 16	//input
#define TRIG 18	//output

void *ultrasonicDetection(void * param);
void usInit();
int getCM();
void usTerm();

#endif
