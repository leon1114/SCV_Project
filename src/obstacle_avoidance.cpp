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
	  장애물 감지 후 차선을 벗어나는 함수
	  direction 인자가 0 일 때는 오른쪽으로, 1 일 때는 왼쪽으로 벗어난다.

	  본 함수는 주어진 방향으로 차선을 벗어난 후,
	  주어진 방향의 반대 방향에 차선이 보이도록 정렬한 상태로 만들고,
	  pt.x 를 지정한 후 종료되어야 함.
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
	 장애물 회피 후 본 차선으로 되돌아가는 함수.
	 direction 인자가 0 일 때는 왼쪽으로, 1 일 때는 오른쪽으로 돌아간다.
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
	 laneDeparture 함수 호출 이후 한 쪽 차선만을 tracking 하는 함수.
	 direction 인자가 0 일 때는 왼쪽에 있는 차선을 (laneDeparture에서 direction이 0 일때 우측으로 차선 벗어나므로)
	 , 1일 때는 오른쪽에 있는 차선을 tracking 한다.

	 본 함수가 호출 될 때는 주어진 direction 인자 값에 대응하는 방향에 차선이 인식 되어있는 상태여야 함.
	 laneDeparture 함수에서 pt.x 값을 지정한 후 본 함수가 호출 됨.
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
