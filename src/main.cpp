//Include statements
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

#include "motor.h"
#include "car_dir.h"
#include "camera.h"

using namespace cv;
using namespace std;
using namespace xfeatures2d;

int main() {
	//load training image
	unsigned int num = 2;
	Mat object[num] = {imread("speed40.jpg", CV_LOAD_IMAGE_GRAYSCALE), imread("speed10.jpg", CV_LOAD_IMAGE_GRAYSCALE)};
	Camera_Init();
	//SURF Detector, and descriptor parameters
	int minHess = 1000;
	vector<KeyPoint> kpObject[num], kpImage[num];
	Mat desObject[num], desImage[num];


	cv::Ptr<SURF> detector = SURF::create(minHess);
	cv::Ptr<SURF> extractor = SURF::create();
	FlannBasedMatcher matcher;

	for(unsigned int i=0; i<num; i++){
		if (!object[i].data) {
			cout << "Can't open image";
			return -1;
		}
		//SurfFeatureDetector detector(minHess);
		detector->detect(object[i], kpObject[i]);
		//SurfDescriptorExtractor extractor;
		extractor->compute(object[i], kpObject[i], desObject[i]);
		if(kpObject[i].empty() == 1){
			cout << "MinHess probably too high" << endl;
			return -1;
		}
	}

	cout << kpObject[0].size() << endl;
	cout << kpObject[1].size() << endl;

	waitKey(0);

	//video loop
	float thresholdMatchingNN = 0.7;
	unsigned int thresholdGoodMatches = 8;
	//unsigned int thresholdGoodMatchesV[] = { 4, 5, 6, 7, 8, 9, 10 };

	while (true) {
		Mat frame;
		Mat image;
		Mat hsv_image;
		frame = Get_Frame();
//		imshow("CAM", frame);
		cv::medianBlur(frame, frame, 3);
		cvtColor(frame, image, CV_RGB2GRAY);
		cvtColor(frame, hsv_image, cv::COLOR_BGR2HSV);

		cv::Mat lower_red_hue_range;
		cv::Mat upper_red_hue_range;
		cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);
		cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);

		cv::Mat red_hue_image;
		cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);
		cv::GaussianBlur(red_hue_image, red_hue_image, cv::Size(9, 9), 2, 2);

		// Use the Hough transform to detect circles in the combined threshold image
		std::vector<cv::Vec3f> circles;
		circles.clear();
		cv::HoughCircles(red_hue_image, circles, CV_HOUGH_GRADIENT, 1, red_hue_image.rows/8, 100, 20, 0, 0);

		if(circles.size() == 0) {
			//cout << "No RED circles detect" << endl;
			continue;
		}

		//Get the coordinates and the radius of the detect circle.
		cv::Point center(round(circles[0][0]), round(circles[0][1]));
		int radius = round(circles[0][2]);

		//Make the mask for the SURF
		//cv::circle(frame, center, radius, cv::Scalar(0, 255, 0), 5);
		Mat mask = Mat::zeros(image.size(), CV_8U);
		//cout << "center x : " << center.x << " center y " << center.y << " radius " << radius << endl;

		//Checks if the mask breaches the frame.
		if( center.x-radius <= 0 || center.y-radius <= 0) continue;
		if( center.x + (radius*2) >= 320 || center.y + (radius*2) >= 240 ) continue;

		Mat roi(mask, cv::Rect( center.x-radius, center.y-radius, (radius*2), (radius*2)));
		roi = Scalar(255, 255, 255);

		Mat des_image;
		Mat img_matches[num], H;
		vector<KeyPoint> kp_image;
		vector<vector<DMatch> > matches;
		vector<DMatch> good_matches[num];
		vector<Point2f> obj;
		vector<Point2f> scene;
		vector<Point2f> scene_corners(4);
		int flag = 0;
		detector->detect(image, kp_image, mask);
		//cout << "In frame, #features : " << kp_image.size() << endl;
		extractor->compute(image, kp_image, des_image);
		des_image.convertTo(des_image, CV_32F);

		for(unsigned int i=0; i<num; i++){
			desObject[i].convertTo(desObject[i], CV_32F);
			if (desObject[i].empty() || des_image.empty() ) continue;
			matches.clear();
			if(kp_image.size() >= 2 && kpObject[i].size() >=2 ) matcher.knnMatch(desObject[i], des_image, matches, 2);

			for (int j = 0; j < min(des_image.rows - 1, (int)matches.size()); j++) //THIS LOOP IS SENSITIVE TO SEGFAULTS
			{
				if ((matches[j][0].distance
						< thresholdMatchingNN * (matches[j][1].distance))
						&& ((int) matches[j].size() <= 2
								&& (int) matches[j].size() > 0)) {
					good_matches[i].push_back(matches[j][0]);
					cout << i << " push" << endl;
				}
			}
			cout << i << " "<< good_matches[i].size() << endl;

			//Draw only "good" matches
			drawMatches(object[i], kpObject[i], image, kp_image, good_matches[i],
					img_matches[i], Scalar::all(-1), Scalar::all(-1),
					vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
			if (good_matches[i].size() >= thresholdGoodMatches) {
				//Display that the object is found
				flag = 1;
				putText(img_matches[i], "Object Found", cvPoint(10, 50),
						FONT_HERSHEY_COMPLEX_SMALL, 2, cvScalar(0, 0, 250), 1,
						CV_AA);
				for (unsigned int j = 0; j < good_matches[i].size(); j++) {
					//Get the keypoints from the good matches
					obj.push_back(kpObject[i][good_matches[i][j].queryIdx].pt);
					scene.push_back(kp_image[good_matches[i][j].trainIdx].pt);
				}
				//Show detected matches
			}
			else {
				putText(img_matches[i], "", cvPoint(10, 50),
						FONT_HERSHEY_COMPLEX_SMALL, 3, cvScalar(0, 0, 250), 1,
						CV_AA);
			}
		}
		if(flag){
			unsigned int max = 0;
			int maxi = 0;
			for( unsigned int i = 0; i< num ; i++){
				if( good_matches[i].size() > max){
					max = good_matches[i].size();
					maxi = i;
				}
			}
			cout << "1st pic matches : "  << good_matches[0].size() << " 2nd pic matches : " << good_matches[1].size() << endl;
			cout << maxi << " is chosen" << endl;
			imshow("Good Matches", img_matches[maxi]);
			cvWaitKey(10);
		}
	}
	return 0;
}

