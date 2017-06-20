#ifndef OBSTACLE_AVOIDANCE_H
#define OBSTACLE_AVOIDANCE_H

#define AVOID_RIGHT 0
#define AVOID_LEFT 1
#define AVOID_SPEED 35
#define AVOID_WIDTH 450

void laneDeparture(int direction);
int singleLaneTracking(int direction);
void laneReturn(int direction);
void manageObstacle();

#endif
