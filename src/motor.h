#ifndef _MOTOR_H
#define _MOTOR_H
#include "PCA9685.h"

#define Motor0_A 0 //11  // pin11
#define Motor0_B 1 //12  // pin12
#define Motor1_A 2 //13  // pin13
#define Motor1_B 3 //15  // pin15

#define EN_M0    4 	// servo driver IC CH4
#define EN_M1    5 	// servo driver IC CH5

int getSpeed();
void setSpeed(int);
void motorInit(void);
void motor0(int);
void motor1(int);
void forward();
void backward();
void forwardWithSpeed(int);
void backwardWithSpeed(int);
void stop(void);
void ctrl(int, int);
void motorTest(void);
void motorTerm(void);

#endif
