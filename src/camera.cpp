#include "camera.h"

using namespace cv;
using namespace std;

raspicam::RaspiCam_Cv Camera;
Mat image;

void Camera_Init()
{
	Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	Camera.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
	Camera.open();
}

Mat Get_Frame()
{
	Camera.grab();
	Camera.retrieve(image);
	//cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
	return image;
}

