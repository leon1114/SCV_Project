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
Mat grayImg;
Mat binaryImg;
Scalar leftVal, rightVal;
int width=450, turndx;
int isOnCorner;
int cornerFrameCounter;
int roadEnded;

int laneKeepingControl()
{
	int leftLaneCord = CORD_NOT_SET, rightLaneCord = CORD_NOT_SET;
	volatile int i;
	pt.y = INITIAL_Y - getSpeed()*0.6 + 20;
	img = getFrame().clone();
	cvtColor(img, grayImg, COLOR_BGR2GRAY);
	threshold(grayImg, binaryImg, 150,255, THRESH_BINARY|THRESH_OTSU);

	//Stop line detect && stop
    if (!isOnCorner && binaryImg.at<uchar>(pt.y - getSpeed() * 0.5 , pt.x) != 0)
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
		if (pt.x - i >= 0) leftVal = binaryImg.at<uchar>(pt.y, pt.x - i);
		if (pt.x + i <= 640) rightVal = binaryImg.at<uchar>(pt.y, pt.x + i);
		if (leftLaneCord == CORD_NOT_SET && leftVal.val[0] != 0)
		{
			leftLaneCord = pt.x - i;
		}
		if (rightLaneCord == CORD_NOT_SET && rightVal.val[0] != 0)
		{
			rightLaneCord = pt.x + i;
		}
		if(leftLaneCord!=CORD_NOT_SET && rightLaneCord!=CORD_NOT_SET) break;
	}

	//mid-lane track
	if (leftLaneCord != CORD_NOT_SET && rightLaneCord != CORD_NOT_SET)
	{
		fineTurn((int)((pt.x - INITIAL_X)/4.0));
		if (cornerFrameCounter <= 0)
		{
			isOnCorner = 0;
		}
		else
		{
			cornerFrameCounter--;
		}
		//width = right_lane_cord - left_lane_cord;
	}
	//turn right
	else if (leftLaneCord != CORD_NOT_SET)
	{
		turndx = (int)(((2*leftLaneCord+width)/2 - INITIAL_X)/2.0)>135?135:(int)(((2*leftLaneCord+width)/2 - INITIAL_X)/2.0);
		fineTurn(turndx);
		isOnCorner = 1;
		cornerFrameCounter = 5;
	}
	//turn left
	else if (rightLaneCord != CORD_NOT_SET)
	{
		turndx = (int)(((2*rightLaneCord-width)/2 - INITIAL_X)/2.0)<-135?-135:(int)(((2*rightLaneCord-width)/2 - INITIAL_X)/2.0);
		fineTurn(turndx);
		isOnCorner = 1;
		cornerFrameCounter = 5;
	}
	//lane end
	else
	{
		roadEnded = 1;
		printf("ended\n");
	}

	//Draw circles inside of each lane
#ifdef SHOW_CAMERA_VISION
	if (leftLaneCord != CORD_NOT_SET) circle(img, Point(leftLaneCord, pt.y), 10, Scalar(0,0,255),-1,8);
	if (rightLaneCord != CORD_NOT_SET) circle(img, Point(rightLaneCord, pt.y), 10, Scalar(0,0,255),-1,8);
	//imshow("test", img);
#endif

	//Init l-r lane cord
	if (leftLaneCord == CORD_NOT_SET) leftLaneCord = rightLaneCord - width;
	if (rightLaneCord == CORD_NOT_SET) rightLaneCord = leftLaneCord + width;

	//Update point x-axis
	pt.x = (leftLaneCord + rightLaneCord) / 2.0;

#ifdef RECORD
	//writer.write(img);
#endif

	//No lane detected
	if (roadEnded)
	{
		//imshow("test", gray_img);
		return 1;
	}
	return 0;
}

