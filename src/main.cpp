
#include <stdio.h>
#include <pthread.h>
#include "motor.h"
#include "car_dir.h"
#include "camera.h"
#include "lanekeeping.h"

using namespace cv;
using namespace std;
#define BASIC_SPEED 50

void Init()
{
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

	//ret = pthread_create(&threadId, NULL, US, (void *)msgThread);
	while(1)
	{
		laneKeepingControl();

//		if (usflag == 1){
//			int prevSpeed = getSpeed();
//			stop();
//			while(usflag);
//			forwardWithSpeed(prevSpeed);
//		}
		if(cv::waitKey(20) == 27) break;
	}

    return 0;
}

