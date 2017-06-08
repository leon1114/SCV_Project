
#include <stdio.h>
#include <pthread.h>
#include "motor.h"
#include "car_dir.h"
#include "camera.h"

cv::Mat imagef;

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
	//return NULL;
}
volatile int usflag;

int main(int argc, char *argv[])
{

	int ret;
	pthread_t threadId;
	const char *msgThread = "Ultrasonic detection thread";
	void *threadRet;


	Init();
	// ========================================

	ret = pthread_create(&threadId, NULL, US, (void *)msgThread);
	while(1)
	{
		imagef = getFrame();

		/******** CV analysis start ********/


		/******** CV analysis finish ********/

		if (usflag == 1){
			int prevSpeed = getSpeed();
			stop();
			while(usflag);
			forwardWithSpeed(prevSpeed);
		}

		/******** Car control start********/


		/******** Car control finish ********/



		if(cv::waitKey(20) == 27) break;
	}



	Terminate();
    return 0;
}

