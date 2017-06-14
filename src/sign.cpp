//Include statements

#include "sign.h"
#include "motor.h"
#include "car_dir.h"
#include "camera.h"

static Mat object[NUM];
static vector<KeyPoint> kpObject[NUM];
static Mat desObject[NUM];
static const string file[NUM] = { "speed40.jpg", "speed100.jpg"};

volatile int fromSignSpd=-1;

int signRecogInit(){

	for (unsigned int i = 0; i < NUM; i++) {
		object[i] = imread(file[i], CV_LOAD_IMAGE_GRAYSCALE);
	}

	Ptr<SURF> detector = SURF::create(minHess);
	Ptr<SURF> extractor = SURF::create();

	for(unsigned int i=0; i<NUM; i++){
		if (object[i].empty()) {
			cout << "Can't open image";
			return -1;
		}
		//SurfFeatureDetector detector(minHess);
		detector->detect(object[i], kpObject[i]);
		if(kpObject[i].empty()){
			cout << "MinHess probably too high for "<< i << endl;
			return -1;
		}
		//SurfDescriptorExtractor extractor;
		extractor->compute(object[i], kpObject[i], desObject[i]);
		if(desObject[i].empty()){
			cout << "empty desObject"<< i << endl;
			return -1;
		}

		desObject[i].convertTo(desObject[i], CV_32F);
	}

	return 0;
}

void redExtract(InputArray src, OutputArray dst){

	Mat ycrcb;
	vector<Mat> ycrcbPlanes;

	cvtColor(src, ycrcb, CV_BGR2YCrCb);
	split(ycrcb, ycrcbPlanes);
	threshold(ycrcbPlanes[2], dst, 150, 255, CV_THRESH_BINARY);
	erode(dst, dst, Mat());

}

int createMask(vector<Vec3f> circles, Mat& mask, Size size){

	int ptx = (int)circles[0][0], pty= (int)circles[0][1], radius = (int)circles[0][2];
	if (ptx - radius < 0 || pty-radius < 0 || ptx + 2*radius >= CAMWIDTH || pty + 2*radius >= CAMHEIGHT) return -1;

	mask = Mat::zeros(size, CV_8U);
	Mat roi(mask, cv::Rect( ptx-radius, pty-radius, (2*radius), (2*radius)));
	roi = Scalar(255, 255, 255);

	return 0;
}

int bestMatch(const Mat& frame, const Mat& mask){

	Ptr<SURF> detector = SURF::create(minHess);
	Ptr<SURF> extractor = SURF::create();

	Mat desFrame;
	vector<KeyPoint> kpFrame;
	vector<vector<DMatch> > matches;
	vector<DMatch> goodMatches[NUM];
	FlannBasedMatcher matcher;

	int flag=0;
	Mat frameMatches[NUM];

	unsigned int max = 0;
	unsigned int detectedObject = 0;

	detector->detect(frame, kpFrame, mask);
	if (kpFrame.empty()){
		cout << "empty kpFrame" << endl;
		return -1;
	}
	else cout << "KeyPoints : " << kpFrame.size() << endl;

	extractor->compute(frame, kpFrame, desFrame);
	if (desFrame.empty()){
		cout << "empty desFrame" << endl;
		return -1;
	}

	desFrame.convertTo(desFrame, CV_32F);

	for (unsigned int i=0; i < NUM; i++){

		matches.clear();
		if (kpFrame.size() >=2 && kpObject[i].size() >=2)  matcher.knnMatch(desObject[i], desFrame, matches, 2);

		for (int j = 0; j < min(desFrame.rows - 1, (int)matches.size()); j++){ //THIS LOOP IS SENSITIVE TO SEGFAULTS

			if ((matches[j][0].distance	< thresholdMatchingNN * (matches[j][1].distance))
					&& ((int) matches[j].size() <= 2 && (int) matches[j].size() > 0)) {
				goodMatches[i].push_back(matches[j][0]);
			}
		}

		if (goodMatches[i].size() >= thresholdGoodMatches) flag = 1;

	}

	if(flag){

		for( unsigned int i = 0; i< NUM ; i++){
			if( goodMatches[i].size() > max){
				max = goodMatches[i].size();
				detectedObject = i;
			}
		}
		cout << "1st pic matches : "  << goodMatches[0].size() << " 2nd pic matches : " << goodMatches[1].size() << endl;
		cout << detectedObject << " is chosen" << endl;

	}
	else return -1;

	return detectedObject;
}

void signRecog(void){

//#define VIEW

	int ret;
	Mat frame, gray, redBinary;
	cout << "signRecog thread has been created" << endl;

	while(true){

		waitKey(10);

		frame = getFrame().clone();

		/******* EXTRACT RED CIRCULAR OBJECT IN FRAME START *******/

		//extract red area in frame
		cvtColor(frame, gray, CV_BGR2GRAY);
		redExtract(frame, redBinary);

		//circle detect
		vector<Vec3f> circles;
		HoughCircles(redBinary, circles, CV_HOUGH_GRADIENT, 1, redBinary.rows/8, 100, 20, 0, 0);
		if (circles.size() < 1) {
			cout << "No red traffic sign in frame" << endl;
			continue;
		}
		else cout << "detect " << circles.size() << " circles" << endl;

		//create mask
		Mat mask;
		if (createMask(circles, mask, redBinary.size()) == -1){
			cout << "Out of frame" << endl;
			continue;
		}

		/******* EXTRACT RED CIRCULAR OBJECT IN FRAME END *******/

		//Matching
		ret = bestMatch(frame, mask);
		if (ret >= 0) {
			fromSignSpd = ret;
			cout << "CHANGE SPEED " << fromSignSpd << endl;
		}

	}

}
