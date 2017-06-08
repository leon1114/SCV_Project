
#include <stdio.h>
#include <pthread.h>
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
	motorInit();
	dirInit();
}

void Terminate()
{
	motorTerm();
	dirTerm();
}

void *US(void * param){
	printf("ultrasonic detection part");
	return NULL;
}
volatile int usflag;

int main(int argc, char *argv[])
{

	int ret;
	pthread_t threadId;
	const char *msgThread = "Ultrasonic detection thread";
	void *threadRet;


	Init();
	forward();
	// ========================================
	printf("lane test INIT\n");

	//ret = pthread_create(&threadId, NULL, US, (void *)msgThread);
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
        	setSpeed(25);
        }
        else if (right_lane_dist)
        {
        	turnLeft();
        	setSpeed(25);
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

		/******** CV analysis start ********/


		/******** CV analysis finish ********/

//		if (usflag == 1){
//			int prevSpeed = getSpeed();
//			stop();
//			while(usflag);
//			forwardWithSpeed(prevSpeed);
//		}

		/******** Car control start********/


		/******** Car control finish ********/

		if(cv::waitKey(20) == 27) break;
	}



	Terminate();
    return 0;
}

