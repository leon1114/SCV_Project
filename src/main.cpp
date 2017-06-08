
#include <stdio.h>

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

volatile int usflag;

int main(int argc, char *argv[])
{
#if 01
	// pi camera 비디오 피드 받아오는 코드
	Init();
	// ========================================

	while(1)
	{
		imagef = getFrame();


		/******** CV analysis start ********/


		/******** CV analysis ended ********/

		if (usflag == 1){
			int prev_speed = getSpeed();


		}
		if(cv::waitKey(20) == 27) break;
	}

#endif
	motorTest();
	dirTest();

	Terminate();
    return 0;
}

