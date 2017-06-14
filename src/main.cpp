
#include <stdio.h>
#include <pthread.h>
#include "motor.h"
#include "car_dir.h"
#include "camera.h"
#include "lanekeeping.h"
#include "ultrasonic.h"
#include "wiringPi.h"
#include "us_dir.h"
#include "obstacle_avoidance.h"

using namespace cv;
using namespace std;
#define BASIC_SPEED 100
#define ESC_KEY 27
#define LK_END 1

extern int dist;

void Init()
{
	wiringPiSetup();
	cameraInit();
	motorInit();
	dirInit();
	usInit();
	usdirInit();
	videoCaptureInit();
}

void Terminate()
{
	motorTerm();
	dirTerm();
	usTerm();
	usdirTerm();
}


extern volatile int usflag;

int main(int argc, char *argv[])
{

	int ret;
	pthread_t threadId;
	const char *msgThread = "Ultrasonic detection thread";
	void *threadRet;

	Init();

	seeHome();
	forwardWithSpeed(BASIC_SPEED);

	ret = pthread_create(&threadId, NULL, ultrasonicDetection, (void *)msgThread);
	if (ret){
		printf("Cannot Open Thread for US\n");
		return -1;
	}

	while(true)
	{
		ret = laneKeepingControl();
		if(ret == LK_END) break;
		if (usflag == 1){
			int prevSpeed = getSpeed();
			stop();
			laneDeparture(1);
			seeRight(90);

			while(dist <= 20) singleLaneTracking(1);
			seeHome();

			laneReturn(1);
			usflag = 0;
			setSpeed(prevSpeed);
//			while(usflag);
//			forwardWithSpeed(prevSpeed);
		}
		if(cv::waitKey(20) == ESC_KEY) break;
	}

	printf("Terminate\n");
	pthread_exit(NULL);
	Terminate();

	waitKey(0);
	return 0;
}

