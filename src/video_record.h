#ifndef VIDEO_WRITER_H
#define VIDEO_WRITER_H
#include "camera.h"

void videoCaptureInit();
void videoFrameWrite(cv::Mat img);

#endif
