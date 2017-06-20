/*
 * sign.h
 *
 *  Created on: 2017. 6. 14.
 *      Author: student
 */

#ifndef SIGN_H_
#define SIGN_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/xfeatures2d.hpp"

#define NUM 2
#define minHess 400// ³·À»¼ö·Ï feature ¸¹ÀÌ Ã£À½.
#define thresholdMatchingNN 0.7 // ³·À»¼ö·Ï ºýºýÇÏ°Ô ¸Â´Ù°íÇÔ.
#define thresholdGoodMatches 1

using namespace cv;
using namespace std;
using namespace xfeatures2d;

int signRecogInit();
void redExtract(InputArray, OutputArray);
int createMask(vector<Vec3f>, Mat&, Size);
int bestMatch(const Mat&, const Mat&);
void *signRecog(void*);


#endif /* SIGN_H_ */
