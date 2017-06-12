#ifndef LANE_KEEPING_H
#define LANE_KEEPING_H

#define CORD_NOT_SET 65536
#define INITIAL_X 320
#define INITIAL_Y 360

void videoCaptureInit(void);
int laneKeepingControl(void);

#endif
