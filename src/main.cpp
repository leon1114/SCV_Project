
#include <stdio.h>

#include "motor.h"
#include "car_dir.h"
#include "camera.h"

using namespace cv;
using namespace std;

Mat img;

// setting variables
Point pt;
Mat gray_img;

Scalar left_val;
Scalar right_val;
int left_lane_dist = 0;
int right_lane_dist = 0;
int road_ended = 0;


void Init()
{
	pt.x = 327;
	pt.y = 350;

	cameraInit();
	motorInit(1, 0x40);
	dirInit(1, 0x40);
}

int main(int argc, char *argv[])
{
#if 01
	// pi camera 비디오 피드 받아오는 코드
	Init();
	forward();
	// ========================================
	printf("lane test INIT\n");

	while(1)
	{
        int left_lane_dist = 0;
        int right_lane_dist = 0;
        volatile int i;
		img = getFrame();
        cvtColor(img, gray_img, COLOR_BGR2GRAY);
        inRange(gray_img,200,255,gray_img);

        for (i=1;i<327;i++)
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


        if (left_lane_dist && right_lane_dist)
        {
        	printf("forward!!");
        	home();
        	setSpeed(25);
        }
        else if (left_lane_dist)
        {
        	turnRight();
        	setSpeed(20);
        }
        else if (right_lane_dist)
        {
        	turnLeft();
        	setSpeed(20);
        }
        else
        {
        	road_ended = 1;
        	printf("ended\n");
        }

        if(road_ended) break;

        printf("Dist to left lane : %d, Dist to right lane : %d\n", left_lane_dist, right_lane_dist);
        if (left_lane_dist) circle(img, Point(pt.x - left_lane_dist, 350), 10, Scalar(0,0,255),-1,8);
        if (right_lane_dist) circle(img, Point(pt.x + right_lane_dist, 350), 10, Scalar(0,0,255),-1,8);

		imshow("test", img);
		if(cv::waitKey(20) == 27) break;
	}

#endif

	motorTerm();
	dirTerm();

    return 0;
}

