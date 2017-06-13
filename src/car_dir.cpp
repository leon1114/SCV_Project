
#include <wiringPi.h>
#include <stdio.h>
#include "car_dir.h"
#include "PCA9685.h"

#define HOMEVAL 900

int leftPWM, rightPWM, homePWM;
static PCA9685 * pwm;

int Map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x-in_min) * (out_max-out_min) / (in_max-in_min) + out_min;
}

void dirInit()
{
	int offset = 0;
	homePWM = HOMEVAL;
	homePWM += offset;

	pwm = new PCA9685(1, 0x40);
	pwm->setPWMFreq(120);
	home();
}

void home()
{
	pwm->setPWM(CH0,0,homePWM);
}

void fineTurn(int dx)
{
	pwm->setPWM(CH0,0,HOMEVAL+dx);
}

void dirTest()
{
	int i;
	dirInit();
	printf("dir-started!\n");
	for(i=0; i<3; i++){
		delay(1000);
		home();
		delay(1000);
		delay(1000);
		home();
	}
	printf("dir-ended!\n");
}

void dirTerm(void)
{
	home();
	pwm->setPWM(CH0, 0, 0); //servo moter power off
	delete pwm;
}
