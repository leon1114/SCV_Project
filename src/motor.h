#ifndef _MOTOR_H
#define _MOTOR_H
#include "PCA9685.h"

#define Motor0_A 11  // pin11
#define Motor0_B 12  // pin12
#define Motor1_A 13  // pin13
#define Motor1_B 15  // pin15

#define EN_M0    4 	// servo driver IC CH4
#define EN_M1    5 	// servo driver IC CH5

void setSpeed(int);
void setup(int, int);
void motor0(int);
void motor1(int);
void forward();
void backward();
void forwardWithSpeed(int);
void backwardWithSpeed(int);
void stop(void);
void ctrl(int, int);
void test(void);

#endif
