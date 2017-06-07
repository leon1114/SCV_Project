#ifndef _CAR_DIR_H
#define _CAR_DIR_H

#define CH0 0

int Map(int x, int in_min, int in_max, int out_min, int out_max);
void dir_setup(int bus, int address);
void turn_left();
void turn_right();
void turn(int angle);
void home();
void calibrate(int dx);
void dir_test();

#endif
