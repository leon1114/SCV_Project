#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<raspicam/raspicam_cv.h>
#include <stdio.h>

using namespace cv; 
using namespace std;
#include "motor.h"
#include "car_dir.h"

int low_r = 100, log_g = 100, low_b = 20, high_r = 255, high_g = 255, high_b = 30;
int low_threshold = 50;
int high_threshold = 150;
int ignore_mask_color = 255;

Mat region_of_interest(Mat image)
{
    int wid = image.cols;
    int hei = image.rows;
    Mat mask = image.clone();
    mask.setTo(0);
    Mat result;
    Point poly_points[1][4];

    poly_points[0][0] = Point(wid/9, hei);
    poly_points[0][1] = Point(wid - wid/9, hei);
    poly_points[0][2] = Point(wid/2 + wid/8, hei/2 + hei/10);
    poly_points[0][3] = Point(wid/2 - wid/8, hei/2 + hei/10);

    const Point* ppt[1] = { poly_points[0] };
    int npt[] = {4};

    fillPoly(mask, ppt, npt, 1, ignore_mask_color);
    bitwise_and(image, mask, result);

    return result;
}

double get_slope(int x1, int x2, int y1, int y2)
{
    int y = y2 - y1;
    int x = x2 - x1;
    return (double)y/(double)x;
}

Mat Convert_Image(Mat img_original)
{
    Mat img_gray, img_hsv, img_mask_yw, mask_yellow, mask_white, mask_yw, gauss_gray, img_canny, img_roi, img_line;
    int num_l_lines = 0;
    int num_r_lines = 0;
    int y_min = img_original.size().height;
    int y_max = (int)(y_min * 0.611);
    double l_x1 = 0;
    double r_x1 = 0;
    double l_x2 = 0;
    double r_x2 = 0;
    double slope;
    double b;

    img_line = img_original.clone();
    img_line.setTo(0);
    img_original = region_of_interest(img_original);
    cvtColor(img_original, img_gray, COLOR_BGR2GRAY);
    cvtColor(img_original, img_hsv, COLOR_BGR2HSV);

    inRange(img_hsv, Scalar(low_b, log_g, low_r), Scalar(high_b, high_g, high_r), mask_yellow);
    inRange(img_gray, 200, 255, mask_white);

    bitwise_or(mask_white, mask_yellow, mask_yw);
    bitwise_and(img_gray, mask_yw, img_mask_yw);

    GaussianBlur(img_mask_yw, gauss_gray, Size(5,5), 0, 0);

    Canny(gauss_gray, img_canny, low_threshold, high_threshold);

    img_roi = region_of_interest(img_canny);

    vector<Vec4i> lines;
    HoughLinesP(img_roi,lines,4,CV_PI/180,30,100,180);

    for (int i=0; i<lines.size();i++)
    {
        Vec4i L = lines[i];
        slope = get_slope(L[0],L[2],L[1],L[3]);
        if (slope > 0)
        {
            b = L[1] - (slope * L[0]);
            r_x1 += (y_min - b)/slope;
            r_x2 += (y_max - b)/slope;
            num_r_lines++;
        }
        else if (slope < 0)
        {
            b = L[1] - (slope * L[0]);
            l_x1 += (y_min - b)/slope;
            l_x2 += (y_max - b)/slope;
            num_l_lines++;
        }
    }

    line(img_line, Point(l_x1 / num_l_lines, y_min), Point((l_x2 / num_l_lines), y_max), Scalar(0,0,255),15,LINE_AA);
    line(img_line, Point(r_x1 / num_r_lines, y_min), Point(r_x2 / num_r_lines, y_max), Scalar(0,0,255),15,LINE_AA);

    addWeighted(img_line, 0.9, img_original, 1, 0, img_original);


    return img_original;
}

int main(int argc, char *argv[])
{
#if 0
	// pi camera 비디오 피드 받아오는 코드
	raspicam::RaspiCam_Cv Camera;
	cv::Mat image;

	Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	Camera.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
	Camera.open();
	// ========================================

	while(1)
	{
		Camera.grab();
		Camera.retrieve(image);
		cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

		cv::imshow("test", image);
		if(cv::waitKey(20) == 27) break;
	}

#endif
	motor_test();
	dir_test();

	int i;
#if 0
    VideoCapture cap1(argv[1]);
    cap1.set(CAP_PROP_FRAME_WIDTH, 640);
    cap1.set(CAP_PROP_FRAME_HEIGHT, 480);

    Mat frame1;
    //namedWindow("converted video", 1);
    namedWindow("original video", 1);

    for (;;)
    {
    	//cnt++;
    	//if (cnt % 3 != 0) continue;
        cap1 >> frame1;
        imshow("original video", frame1);
        //frame1 = Convert_Image(frame1);
        //imshow("converted video", frame1);
        if (waitKey(30) == 27) break;
    }
#endif // 0

    return 0;
}

