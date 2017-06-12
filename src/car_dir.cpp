
#include <wiringPi.h>
#include <stdio.h>
#include "car_dir.h"
#include "PCA9685.h"


int leftPWM, rightPWM, homePWM;
static PCA9685 * pwm;

int Map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x-in_min) * (out_max-out_min) / (in_max-in_min) + out_min;
}

void dirInit()
{
	int offset = 0;
	leftPWM = 380, homePWM = 450, rightPWM = 520;
	leftPWM += offset;
	homePWM += offset;
	rightPWM += offset;

	pwm = new PCA9685(1, 0x40);
	pwm->setPWMFreq(60);
	home();
}

void turnLeft()
{
	pwm->setPWM(CH0,0,leftPWM);
}

void turnRight()
{
	pwm->setPWM(CH0,0,rightPWM);
}

void turn(int angle)
{
	angle = Map(angle, 0, 255, leftPWM, rightPWM);
	pwm->setPWM(CH0,0,angle);
}

void home()
{
	pwm->setPWM(CH0,0,homePWM);
}

//
void fineTurn(int dx)
{
	pwm->setPWM(CH0,0,450+dx);
}

void dirTest()
{
	int i;
	dirInit();
	printf("dir-started!\n");
	for(i=0; i<3; i++){
		turnLeft();
		delay(1000);
		home();
		delay(1000);
		turnRight();
		delay(1000);
		home();
	}
	printf("dir-ended!\n");
}

void dirTerm(void)
{
	home();
	delete pwm;
}
