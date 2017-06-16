#include "car_dir.h"
#include "obstacle_avoidance.h"
#include "lanekeeping.h"
#include "camera.h"
#include "motor.h"
#include "wiringPi.h"
#include "us_dir.h"
#include "ultrasonic.h"

#define turnLeft() (fineTurn(-135))
#define turnRight() (fineTurn(135))

using namespace cv;
using namespace std;

extern Point pt;
extern VideoWriter writer;
int camera_turned;
extern int angle;

int leftTurned;
int rightTurned;

void laneReturn(int direction)
{
	/*
	 ��ֹ� ȸ�� �� �� �������� �ǵ��ư��� �Լ�.
	 direction ���ڰ� 0 �� ���� ��������, 1 �� ���� ���������� ���ư���.
	 */

	setSpeed(0);
	angle = 0;
	camera_turned = 0;
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
		if (pt.x + i * multiplier < 0 || pt.x + i * multiplier > 640)
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

	if (!camera_turned && lane_cord <= 50 && (turndx >= -20 && turndx <= 20)&& rightTurned && leftTurned) {
		setSpeed(0);
		angle = 90;
		delay(500);
		setSpeed(AVOID_SPEED);
		camera_turned = 1;
		rightTurned = 0;
		leftTurned = 0;
	}

	if (lane_cord != CORD_NOT_SET)
	{
		turndx = (int)(((2*lane_cord + (AVOID_WIDTH * multiplier * (-1)))/2 - INITIAL_X)/3)>90?90:(int)(((2*lane_cord+(AVOID_WIDTH * multiplier * (-1)))/2 - INITIAL_X)/3);
		fineTurn(turndx);
		pt.x = lane_cord + (AVOID_WIDTH * multiplier * (-1)) / 2;
	}
	else
	{
		if (direction == AVOID_RIGHT) turnLeft();
		else turnRight();
	}

	if (turndx > 50) rightTurned = 1;
	if (rightTurned && turndx < -20) leftTurned = 1;

	return 0;
}
