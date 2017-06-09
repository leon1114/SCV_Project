#include "lanekeeping.h"
#include "camera.h"
#include "motor.h"
#include "car_dir.h"

using namespace std;
using namespace cv;

#define Y_MAX 360

Mat img;

// setting variables
Point pt;
Mat gray_img;
Scalar left_val, right_val;

void laneKeepingControl()
{
	pt.x = 327;
	pt.y = Y_MAX - getSpeed()/2;
    int left_lane_dist = 0, right_lane_dist = 0, road_ended = 0;
    volatile int i;
	img = getFrame();
    cvtColor(img, gray_img, COLOR_BGR2GRAY);
    inRange(gray_img,180,255,gray_img);

    for (i=1;i<350;i++)
     {
         left_val = gray_img.at<uchar>(pt.y, pt.x - i);
         right_val = gray_img.at<uchar>(pt.y, pt.x + i);
         if (!left_lane_dist &&left_val.val[0] != 0)
         {
             left_lane_dist = i;
         }
         if (!right_lane_dist && right_val.val[0] != 0)
         {
             right_lane_dist = i;
         }
     }

    printf("Dist to left lane : %d, Dist to right lane : %d\n", left_lane_dist, right_lane_dist);
/*
    if (left_lane_dist && right_lane_dist)
    {
    	home();
    	setSpeed(BASIC_SPEED);
    }
    else if (left_lane_dist)
    {
    	turnRight();
    	setSpeed(BASIC_SPEED);
    }
    else if (right_lane_dist)
    {
    	turnLeft();
    	setSpeed(BASIC_SPEED);
    }
    */
    if (left_lane_dist && right_lane_dist)
    {
    	home();
    }
    else if (left_lane_dist)
	{
    	if (left_lane_dist < 270) left_lane_dist=270;
		fineTurn(350-left_lane_dist);
	}
	else if (right_lane_dist)
	{
		if (right_lane_dist < 270) right_lane_dist=270;
		fineTurn(right_lane_dist-350);
	}
    else
    {
    	road_ended = 1;
    	motorTerm();
    	dirTerm();
    	printf("ended\n");
    }


    if (left_lane_dist) circle(img, Point(pt.x - left_lane_dist, 350), 10, Scalar(0,0,255),-1,8);
    if (right_lane_dist) circle(img, Point(pt.x + right_lane_dist, 350), 10, Scalar(0,0,255),-1,8);

	imshow("test", img);
	if (road_ended)
	{
		imshow("test", gray_img);
		waitKey(0);
	}
}
