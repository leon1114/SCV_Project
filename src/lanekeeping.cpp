/**/

#include "lanekeeping.h"
#include "camera.h"
#include "motor.h"
#include "car_dir.h"
#include "wiringPi.h"

#define RECORD

using namespace std;
using namespace cv;

Mat img;

// setting variables
Point pt = Point(INITIAL_X, INITIAL_Y-50);
Mat gray_img;
Scalar left_val, right_val;
VideoWriter writer;
int width=450, turndx;
int isOnCorner;

#ifdef RECORD
void videoCaptureInit()
{
	writer.open("./record.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10.0, Size(640, 360), 1);
}
#endif

int laneKeepingControl()
{
	int left_lane_cord = CORD_NOT_SET, right_lane_cord = CORD_NOT_SET, road_ended = 0;
	volatile int i;
	pt.y = INITIAL_Y - getSpeed();
	img = getFrame();
	cvtColor(img, gray_img, COLOR_BGR2GRAY);
	//inRange(gray_img,150,255,gray_img);
	threshold(gray_img, gray_img, 150,255, THRESH_BINARY|THRESH_OTSU);

	//Stop line detect && stop
    if (!isOnCorner && gray_img.at<uchar>(pt.y, pt.x) != 0)
    {
    	printf("Stop line detected\n");
    	int prv_spd = getSpeed();
    	for (int spd = prv_spd;; spd -= 30)
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

	printf("PT.X : %d , Left lane x cord : %d, Right lane x cord : %d\n", pt.x, left_lane_cord, right_lane_cord);

	//mid-lane track
	if (left_lane_cord != CORD_NOT_SET && right_lane_cord != CORD_NOT_SET)
	{
		fineTurn((pt.x - INITIAL_X)/5);
		isOnCorner = 0;
		//width = right_lane_cord - left_lane_cord;
	}
	//turn right
	else if (left_lane_cord != CORD_NOT_SET)
	{
		turndx = (int)(((2*left_lane_cord+width)/2 - INITIAL_X)/3)>90?90:(int)(((2*left_lane_cord+width)/2 - INITIAL_X)/3);
		fineTurn(turndx);
		isOnCorner = 1;
	}
	//turn left
	else if (right_lane_cord != CORD_NOT_SET)
	{
		turndx = (int)(((2*right_lane_cord-width)/2 - INITIAL_X)/3)<-90?-90:(int)(((2*right_lane_cord-width)/2 - INITIAL_X)/3);
		fineTurn(turndx);
		isOnCorner = 1;
	}
	//lane end
	else
	{
		road_ended = 1;
		motorTerm();
		dirTerm();
		printf("ended\n");
	}

	//Draw circles inside of each lane
	if (left_lane_cord != CORD_NOT_SET) circle(img, Point(left_lane_cord, pt.y), 10, Scalar(0,0,255),-1,8);
	if (right_lane_cord != CORD_NOT_SET) circle(img, Point(right_lane_cord, pt.y), 10, Scalar(0,0,255),-1,8);

	//Init l-r lane cord
	if (left_lane_cord == CORD_NOT_SET) left_lane_cord = 0;
	if (right_lane_cord == CORD_NOT_SET) right_lane_cord = 640;

	//Update point x-axis
	pt.x = (left_lane_cord + right_lane_cord) / 2;

#ifdef RECORD
	writer.write(img);
#endif
	imshow("test", img);

	//No lane detected
	if (road_ended)
	{
		imshow("test", gray_img);
		return 1;
	}
	return 0;
}

