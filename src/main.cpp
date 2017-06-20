
#include <stdio.h>
#include <pthread.h>
#include "motor.h"
#include "car_dir.h"
#include "camera.h"
#include "video_record.h"
#include "lanekeeping.h"
#include "ultrasonic.h"
#include "wiringPi.h"
#include "us_dir.h"
#include "obstacle_avoidance.h"
#include "sign.h"

using namespace cv;
using namespace std;
#define BASIC_SPEED 70
#define ESC_KEY 27
#define LK_END 1

void Init()
{
	wiringPiSetup();
	cameraInit();
	motorInit();
	dirInit();
	usInit();
	usdirInit();
	videoCaptureInit();

#if 0 // 나중에 수정 할거니까 냅두기
	if (signRecogInit() == -1) {
		cout << "signRecogInit failed" << endl;
		exit(0);
	}
#else
	signRecogInit();
#endif

}

void Terminate()
{
	motorTerm();
	dirTerm();
	usTerm();
	usdirTerm();
}


extern volatile int usflag;
extern volatile int fromSignSpd;
extern int angle;

int main(int argc, char *argv[])
{
	int ret;
	Mat camImg;
	pthread_t threadUSId;
	pthread_t threadSRId;
	const char *threadUS = "Ultrasonic detection thread";
	const char *threadSR = "Sign Recognition thread";

	Init();

	seeHome();
	angle = 0;
	forwardWithSpeed(BASIC_SPEED);

	ret = pthread_create(&threadUSId, NULL, ultrasonicDetection, (void *)threadUS);
	if (ret){
		printf("Cannot Open Thread for US\n");
		return -1;
	}

	ret = pthread_create(&threadSRId, NULL, signRecog, (void *)threadSR);
	if (ret){
		printf("Cannot Open Thread for SR\n");
		return -1;
	}

	while(true)
	{
		ret = laneKeepingControl();
		if(ret == LK_END) break;
		if (fromSignSpd){
			setSpeed(fromSignSpd);
			cout << "fromSignSpd : " << fromSignSpd << endl;
			fromSignSpd = 0;
		}

		if (usflag == 1) manageObstacle();
		if(cv::waitKey(20) == ESC_KEY) break;
	}


	printf("Terminate\n");
	Terminate();
	pthread_cancel(threadSRId);
	printf("SR THREAD CANCELED\n");
	pthread_cancel(threadUSId);
	printf("US THREAD CANCELED\n");

	waitKey(0);
	return 0;
}

