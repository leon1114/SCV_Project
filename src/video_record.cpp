#include <stdio.h>
#include "camera.h"
#include "wiringPi.h"
#include "obstacle_avoidance.h"
#include "video_record.h"

using namespace cv;
using namespace std;

VideoWriter writer;

void videoCaptureInit()
{
	writer.open("./record.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10.0, Size(CAMWIDTH, CAMHEIGHT), 1);
}

void videoFrameWrite(Mat img)
{
	writer.write(img);
}
