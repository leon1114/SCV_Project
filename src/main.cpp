#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>

#include "motor.h"
#include "car_dir.h"
#include "camera.h"

using namespace std;
using namespace cv;

//Traffic Recognition related variables
bool use_mask;
Mat img; Mat templ; Mat mask; Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";
int match_method;
int max_Trackbar = 5;

cv::Mat imagef;

void MatchingMethod( int num, void* pointer)
{
  Mat img_display;
  img.copyTo( img_display );

  int result_cols =  img.cols - templ.cols + 1;
  int result_rows = img.rows - templ.rows + 1;

  cout << "imgcol:" <<img.cols << "imgrow:" << img.rows << endl;
  cout << "col:" << result_cols << "row:" << result_rows << endl;

  result.create( result_rows, result_cols, CV_32FC1 );
  bool method_accepts_mask = (CV_TM_SQDIFF == match_method || match_method == CV_TM_CCORR_NORMED);
  if (use_mask && method_accepts_mask)
    { matchTemplate( img, templ, result, match_method, mask); }
  else
    { matchTemplate( img, templ, result, match_method); }

  normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  Point matchLoc;
  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
  if( match_method  == TM_SQDIFF || match_method == TM_SQDIFF_NORMED )
    { matchLoc = minLoc; }
  else
    { matchLoc = maxLoc; }
  rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
  rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
  imshow( image_window, img_display );
  imshow( result_window, result );

  return;
}

int main(int argc, char *argv[])
{
#if 01
	// pi camera 비디오 피드 받아오는 코드
	//Camera_Init();
	// ========================================
	templ = imread(argv[1], 1);
	namedWindow( "image_window", WINDOW_AUTOSIZE );
	cv::imshow("image_window", templ);

#if 0
	while(1)
	{
		//imagef = Get_Frame();
		img = Get_Frame();
		cout << "HI!" << endl;
		namedWindow( image_window, WINDOW_AUTOSIZE );
		namedWindow( result_window, WINDOW_AUTOSIZE );
		cout << "HI2!" << endl;
		//const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
		//createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );
		MatchingMethod( 0, 0 );
		cout << "HI3!" << endl;
		waitKey(30);

		/*
		cv::imshow("test", imagef);
		if(cv::waitKey(20) == 27) break;
		*/
	}
	motor_test();
	dir_test();
#endif

#endif

	waitKey(0);
    return 0;
}

