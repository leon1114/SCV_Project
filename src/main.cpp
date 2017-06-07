
#include <stdio.h>

#include "motor.h"
#include "car_dir.h"
#include "camera.h"

cv::Mat imagef;

int main(int argc, char *argv[])
{
#if 01
	// pi camera ���� �ǵ� �޾ƿ��� �ڵ�
	Camera_Init();
	// ========================================

	while(1)
	{
		imagef = Get_Frame();
		cv::imshow("test", imagef);
		if(cv::waitKey(20) == 27) break;
	}

#endif
	motor_test();
	dir_test();

    return 0;
}

