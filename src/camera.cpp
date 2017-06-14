#include "camera.h"

using namespace cv;
using namespace std;

raspicam::RaspiCam_Cv Camera;

void cameraInit()
{
	Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	Camera.set(CV_CAP_PROP_FRAME_WIDTH, CAMWIDTH);
	Camera.set(CV_CAP_PROP_FRAME_HEIGHT, CAMHEIGHT);
	Camera.open();
}

Mat getFrame()
{
	static Mat image;
	Camera.grab();
	Camera.retrieve(image);
	return image;
}
