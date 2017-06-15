#include "car_dir.h"
#include "obstacle_avoidance.h"
#include "lanekeeping.h"
#include "camera.h"
#include "motor.h"
#include "wiringPi.h"

#define turnLeft() (fineTurn(-135))
#define turnRight() (fineTurn(135))

using namespace cv;
using namespace std;

extern Point pt;
extern int isOnCorner;
extern VideoWriter writer;

void laneDeparture(int direction)
{
	/*
	  ��ֹ� ���� �� ������ ����� �Լ�
	  direction ���ڰ� 0 �� ���� ����������, 1 �� ���� �������� �����.

	  �� �Լ��� �־��� �������� ������ ��� ��,
	  �־��� ������ �ݴ� ���⿡ ������ ���̵��� ������ ���·� �����,
	  pt.x �� ������ �� ����Ǿ�� ��.
	 */
	if (direction == AVOID_RIGHT) fineTurn(135);
	else turnLeft();

	forwardWithSpeed(AVOID_SPEED);
	delay(700);


	if (direction == AVOID_RIGHT) turnLeft();
	else turnRight();

	delay(700);
	stop();

	return;
}

void laneReturn(int direction)
{
	/*
	 ��ֹ� ȸ�� �� �� �������� �ǵ��ư��� �Լ�.
	 direction ���ڰ� 0 �� ���� ��������, 1 �� ���� ���������� ���ư���.
	 */
	if (direction == AVOID_LEFT) turnRight();
	else  turnLeft();

	forwardWithSpeed(AVOID_SPEED);
	delay(700);

	if (direction == AVOID_LEFT) turnLeft();
	else turnRight();
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
	int turndx;
	Scalar pixel_val;
	int lane_cord = CORD_NOT_SET;
	volatile int i;
	int multiplier = direction == 0 ? -1 : 1;
	pt.y = INITIAL_Y - 50;
	img = getFrame();
	cvtColor(img, gray_img, COLOR_BGR2GRAY);
	inRange(gray_img,200,255,gray_img);

    //Lane detection
	for (i=1;i<=640;i++)
	{
		if (pt.x + i * multiplier < 0 || pt.x + i * multiplier > 640)
		{
			printf("singleLaneTracking Error : Cannot find lane\n");
			break;
		}
		pixel_val = gray_img.at<uchar>(pt.y, pt.x + i * multiplier);

		if (pixel_val.val[0] != 0)
		{
			lane_cord = pt.x + i * multiplier;
			break;
		}
	}

	printf("singleLaneTracking Lane Found, Cord : %d\n", lane_cord);

	if (lane_cord != CORD_NOT_SET)
	{
		turndx = (int)(((2*lane_cord + (AVOID_WIDTH * multiplier * (-1)))/2 - INITIAL_X)/3)>90?90:(int)(((2*lane_cord+(AVOID_WIDTH * multiplier * (-1)))/2 - INITIAL_X)/3);
		fineTurn(turndx);
		pt.x = (lane_cord + AVOID_WIDTH * multiplier * (-1)) / 2;
		circle(img, Point(lane_cord, pt.y), 10, Scalar(0,0,255),-1,8);
	}
	else if (lane_cord == INITIAL_X)
	{
		if (direction == 0) turnRight();
		else turnLeft();
		pt.x = INITIAL_X;
	}
	else
	{
		if (direction == 0) turnLeft();
		else turnRight();
		pt.x = INITIAL_X;
	}
	writer.write(img);
	imshow("test", img);

	return 0;
}
