#ifndef LANE_KEEPING_H
#define LANE_KEEPING_H

#define BASIC_SPEED 60
#define TURN_SPEED 60
#define CORD_NOT_SET 65536
#define INITIAL_X 320
#define INITIAL_Y 360

void videoCaptureInit(void);
void laneKeepingControl(void);

#endif
