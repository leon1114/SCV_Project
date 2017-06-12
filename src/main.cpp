
#include <stdio.h>
#include <pthread.h>
#include "motor.h"
#include "car_dir.h"
#include "camera.h"
#include "lanekeeping.h"
#include "ultrasonic.h"

using namespace cv;
using namespace std;
#define BASIC_SPEED 100
#define ESC_KEY 27
#define LK_END 1


void Init()
{
	cameraInit();
	motorInit();
	dirInit();
	usInit();
	videoCaptureInit();
}

void Terminate()
{
	motorTerm();
	dirTerm();
	usTerm();
}


extern volatile int usflag;

int main(int argc, char *argv[])
{
	int ret;
	pthread_t threadId;
	const char *msgThread = "Ultrasonic detection thread";
	void *threadRet;

	Init();
	forwardWithSpeed(BASIC_SPEED);

	ret = pthread_create(&threadId, NULL, ultrasonicDetection, (void *)msgThread);
	while(true)
	{
		ret = laneKeepingControl();
		if(ret == LK_END) break;
		if (usflag == 1){
			int prevSpeed = getSpeed();
			stop();
			while(usflag);
			forwardWithSpeed(prevSpeed);
		}
		if(cv::waitKey(20) == ESC_KEY) break;
	}
	terminate();
    return 0;
}

