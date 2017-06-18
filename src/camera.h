#ifndef CAMERA_H
#define CAMERA_H
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <raspicam/raspicam_cv.h>

#define RECORD_CAMERA_VISION 1
#define SHOW_CAMERA_VISION 0

#define CAMHEIGHT 360
#define CAMWIDTH 640

void cameraInit();
cv::Mat getFrame();

#endif
