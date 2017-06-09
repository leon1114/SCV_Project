#include "lanekeeping.h"
#include "camera.h"
#include "motor.h"
#include "car_dir.h"

using namespace std;
using namespace cv;

Mat img;

// setting variables
Point pt = Point(INITIAL_X, INITIAL_Y-30);
Mat gray_img;
Scalar left_val, right_val;
VideoWriter writer;

void videoCaptureInit()
{
    writer.open("./record.avi", CV_FOURCC('M', 'J', 'P', 'G'), 15.0, Size(640, 360), 1);
}

void laneKeepingControl()
{
    int left_lane_cord = CORD_NOT_SET, right_lane_cord = CORD_NOT_SET, road_ended = 0;
    volatile int i;
    pt.y = INITIAL_Y - getSpeed()/2;
	img = getFrame();
    cvtColor(img, gray_img, COLOR_BGR2GRAY);
    inRange(gray_img,150,255,gray_img);

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
     }

    printf("PT.X : %d , Left lane x cord : %d, Right lane x cord : %d\n", pt.x, left_lane_cord, right_lane_cord);

    if (left_lane_cord != CORD_NOT_SET && right_lane_cord != CORD_NOT_SET)
    {
    	fineTurn(pt.x - INITIAL_X);
    	setSpeed(BASIC_SPEED);
    }
    else if (left_lane_cord != CORD_NOT_SET)
    {
    	turnRight();
    	setSpeed(TURN_SPEED);
    }
    else if (right_lane_cord != CORD_NOT_SET)
    {
    	turnLeft();
    	setSpeed(TURN_SPEED);
    }
    else
    {
    	road_ended = 1;
    	motorTerm();
    	dirTerm();
    	printf("ended\n");
    }


    if (left_lane_cord != CORD_NOT_SET) circle(img, Point(left_lane_cord, 350), 10, Scalar(0,0,255),-1,8);
    if (right_lane_cord != CORD_NOT_SET) circle(img, Point(right_lane_cord, 350), 10, Scalar(0,0,255),-1,8);

    if (left_lane_cord == CORD_NOT_SET) left_lane_cord = 0;
    if (right_lane_cord == CORD_NOT_SET) right_lane_cord = 640;

    pt.x = (left_lane_cord + right_lane_cord) / 2;

    writer.write(img);
	imshow("test", img);
	if (road_ended)
	{
		imshow("test", gray_img);
		waitKey(0);
	}
}

