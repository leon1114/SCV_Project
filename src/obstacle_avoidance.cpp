#include "car_dir.h"
#include "obstacle_avoidance.h"
#include "lanekeeping.h"
#include "camera.h"
#include "video_record.h"
#include "motor.h"
#include "wiringPi.h"
#include "us_dir.h"
#include "ultrasonic.h"

#define turnLeft() (fineTurn(-135))
#define turnRight() (fineTurn(135))

using namespace cv;
using namespace std;


int cameraTurned;
int obstacleFound;
int leftTurned;
int rightTurned;
extern int angle;
extern int dist;
extern int isOnCorner;
extern Point pt;
extern VideoWriter writer;

void manageObstacle()
{
	Mat camImg;
	int prevSpeed = getSpeed();
	extern int usflag;
	setSpeed(0);
	if (prevSpeed <= 50){

		pt.x += 450; // OBSTACLE AVOIDANCE ���۵��� 560���� �ٲٱ�
		delay(200);
		setSpeed(AVOID_SPEED);

		while (1) {
			singleLaneTracking(0);
			if (cameraTurned && dist >= 40 && obstacleFound)
			{
				obstacleFound = 0;
				isOnCorner = 1;
				break;
			}
			else if (cameraTurned && dist < 40 && !obstacleFound)
			{
				obstacleFound = 1;
			}
			waitKey(20);
		}
		laneReturn(0);
	}
	else
	{
		while(usflag)
		{
	#if RECORD_CAMERA_VISION
			camImg = getFrame().clone();
			videoFrameWrite(camImg);
			waitKey(20);
	#endif
		};
		setSpeed(prevSpeed);
	}
}

void laneReturn(int direction)
{
	/*
	 ��ֹ� ȸ�� �� �� �������� �ǵ��ư��� �Լ�.
	 direction ���ڰ� 0 �� ���� ��������, 1 �� ���� ���������� ���ư���.
	 */

	setSpeed(0);
	angle = 0;
	cameraTurned = 0;
	if (direction == AVOID_LEFT) turnRight();
	else  turnLeft();

	forwardWithSpeed(AVOID_SPEED);
	delay(1000);

	if (direction == AVOID_LEFT) turnLeft();
	else turnRight();

	delay(100);
}

int singleLaneTracking(int direction)
{
	/*
	 laneDeparture �Լ� ȣ�� ���� �� �� �������� tracking �ϴ� �Լ�.
	 direction ���ڰ� 0 �� ���� ���ʿ� �ִ� ������ (laneDeparture���� direction�� 0 �϶� �������� ���� ����Ƿ�)
	 , 1�� ���� �����ʿ� �ִ� ������ tracking �Ѵ�.

	 �� �Լ��� ȣ�� �� ���� �־��� direction ���� ���� �����ϴ� ���⿡ ������ �ν� �Ǿ��ִ� ���¿��� ��.
	 laneDeparture �Լ����� pt.x ���� ������ �� �� �Լ��� ȣ�� ��.
	 */
	Mat gray_img;
	Mat img;
	static int turndx;
	Scalar pixel_val;
	int lane_cord = CORD_NOT_SET;
	volatile int i;
	int multiplier = direction == 0 ? -1 : 1;
	pt.y = INITIAL_Y - 50;
	img = getFrame().clone();
	cvtColor(img, gray_img, COLOR_BGR2GRAY);
	inRange(gray_img,180,255,gray_img);

    //Lane detection
	for (i=1;i<=CAMWIDTH;i++)
	{
		if (pt.x + i * multiplier < 0 || pt.x + i * multiplier > CAMWIDTH)
		{
			continue;
		}
		pixel_val = gray_img.at<uchar>(pt.y, pt.x + i * multiplier);

		if (pixel_val.val[0] != 0)
		{
			lane_cord = pt.x + i * multiplier;
			break;
		}
	}

	if (!cameraTurned && (turndx > -20 && turndx < 20) && rightTurned && leftTurned) {
		setSpeed(0);
		angle = 90;
		delay(500);
		setSpeed(AVOID_SPEED);
		cameraTurned = 1;
		rightTurned = 0;
		leftTurned = 0;
	}

	if (lane_cord != CORD_NOT_SET)
	{
		turndx = (int)(((2*lane_cord+450)/2 - INITIAL_X)/1.5)>135?135:(int)(((2*lane_cord+450)/2 - INITIAL_X)/1.5);
		fineTurn(turndx);
		pt.x = lane_cord + (AVOID_WIDTH * multiplier * (-1)) / 2;
	}
	else
	{
		if (direction == AVOID_RIGHT) turnLeft();
		else turnRight();
	}

	if (turndx > 50) {
		printf("right turned\n");
		rightTurned = 1;
	}
	if (rightTurned && turndx < -15) {
		printf("left turned\n");
		leftTurned = 1;
	}
	printf("AVOIDANCE TURNDX : %d\n", turndx);

#if RECORD_CAMERA_VISION
	videoFrameWrite(img);
#endif

	return 0;
}
