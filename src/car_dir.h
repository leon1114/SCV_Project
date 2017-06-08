#ifndef _CAR_DIR_H
#define _CAR_DIR_H

#define CH0 0

int Map(int x, int in_min, int in_max, int out_min, int out_max);
void dirInit(int bus, int address);
void turnLeft();
void turnRight();
void turn(int angle);
void home();
void calibrate(int dx);
void dirTest();
void dirTerm(void);

#endif
