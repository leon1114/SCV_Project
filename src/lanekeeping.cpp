/**/

#include "lanekeeping.h"
#include "camera.h"
#include "motor.h"
#include "car_dir.h"
#include "wiringPi.h"

#define RECORD
#define SHOW_CAMERA_VISION

using namespace std;
using namespace cv;

Mat img;

// setting variables
Point pt = Point(INITIAL_X, INITIAL_Y-50);
Mat gray_img;
Scalar left_val, right_val;
int width=450, turndx;
int isOnCorner;
int cornerFrameCounter;
int road_ended;

int laneKeepingControl()
{
	int left_lane_cord = CORD_NOT_SET, right_lane_cord = CORD_NOT_SET;
	volatile int i;
	pt.y = INITIAL_Y - getSpeed()*0.8;
	img = getFrame().clone();
	cvtColor(img, gray_img, COLOR_BGR2GRAY);
	//inRange(gray_img,180,255,gray_img);
	threshold(gray_img, gray_img, 180,255, THRESH_BINARY|THRESH_OTSU);

	//Stop line detect && stop
    if (!isOnCorner && gray_img.at<uchar>(pt.y , pt.x) != 0)
    {
    	printf("Stop line detected\n");
    	int prv_spd = getSpeed();
    	for (int spd = prv_spd;; spd -= prv_spd *0.9)
    	{
    		if (spd <= 0)
    		{
    			setSpeed(0);
    			delay(2000);
    			setSpeed(prv_spd);
    			break;
    		}
    		setSpeed(spd);
    		delay(1);
    	}
    }

    //Lane detection
	for (i=1;i<=640;i++)
	{
		if (pt.x - i >= 0) left_val = gray_img.at<uchar>(pt.y, pt.x - i);
		if (pt.x + i <= 640) right_val = gray_img.at<uchar>(pt.y, pt.x + i);
		if (left_lane_cord == CORD_NOT_SET && left_val.val[0] != 0)
		{
			left_lane_cord = pt.x - i;
		}
		if (right_lane_cord == CORD_NOT_SET && right_val.val[0] != 0)
		{
			right_lane_cord = pt.x + i;
		}
		if(left_lane_cord!=CORD_NOT_SET && right_lane_cord!=CORD_NOT_SET) break;
	}

	//mid-lane track
	if (left_lane_cord != CORD_NOT_SET && right_lane_cord != CORD_NOT_SET)
	{
		fineTurn((int)((pt.x - INITIAL_X)/6));
		if (cornerFrameCounter <= 0)
		{
			isOnCorner = 0;
		}
		else
		{
			cornerFrameCounter--;
		}
		width = right_lane_cord - left_lane_cord;
	}
	//turn right
	else if (left_lane_cord != CORD_NOT_SET)
	{
		turndx = (int)(((2*left_lane_cord+width)/2 - INITIAL_X)/1.5)>135?135:(int)(((2*left_lane_cord+width)/2 - INITIAL_X)/1.5);
		fineTurn(turndx);
		cornerFrameCounter++;
		isOnCorner = 1;
		cornerFrameCounter = 100;
	}
	//turn left
	else if (right_lane_cord != CORD_NOT_SET)
	{
		turndx = (int)(((2*right_lane_cord-width)/2 - INITIAL_X)/1.5)<-135?-135:(int)(((2*right_lane_cord-width)/2 - INITIAL_X)/1.5);
		fineTurn(turndx);
		cornerFrameCounter++;
		isOnCorner = 1;
		cornerFrameCounter = 100;
	}
	//lane end
	else
	{
		road_ended = 1;
		printf("ended\n");
	}

	//Draw circles inside of each lane
#ifdef SHOW_CAMERA_VISION
	if (left_lane_cord != CORD_NOT_SET) circle(img, Point(left_lane_cord, pt.y), 10, Scalar(0,0,255),-1,8);
	if (right_lane_cord != CORD_NOT_SET) circle(img, Point(right_lane_cord, pt.y), 10, Scalar(0,0,255),-1,8);
	//imshow("test", img);
#endif

	//Init l-r lane cord
	if (left_lane_cord == CORD_NOT_SET) left_lane_cord = right_lane_cord - width;
	if (right_lane_cord == CORD_NOT_SET) right_lane_cord = left_lane_cord + width;

	//Update point x-axis
	pt.x = (left_lane_cord + right_lane_cord) / 2.0;

#ifdef RECORD
	//writer.write(img);
#endif

	//No lane detected
	if (road_ended)
	{
		imshow("test", gray_img);
		return 1;
	}
	return 0;
}

