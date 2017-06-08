
#include <stdio.h>

#include "motor.h"
#include "car_dir.h"
#include "camera.h"

cv::Mat imagef;

void Init()
{
	cameraInit();
	motorInit(1, 0x40);
	dirInit(1, 0x40);
}

int main(int argc, char *argv[])
{
#if 01
	// pi camera 비디오 피드 받아오는 코드
	Init();
	// ========================================

	while(1)
	{
		imagef = getFrame();
		cv::imshow("test", imagef);
		if(cv::waitKey(20) == 27) break;
	}

#endif
	motorTest();
	dirTest();

    return 0;
}

