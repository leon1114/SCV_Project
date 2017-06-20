
#include <wiringPi.h>
#include <stdio.h>
#include "car_dir.h"
#include "PCA9685.h"

int leftPWM, rightPWM, homePWM;
int offset = -7;
static PCA9685 * pwm;

void dirInit()
{
	homePWM = HOMEVAL;
	homePWM += offset;

	pwm = new PCA9685(1, 0x40);
	pwm->setPWMFreq(90);
	home();
}

void home()
{
	pwm->setPWM(CH0,0,homePWM);
}

void fineTurn(int dx)
{
	pwm->setPWM(CH0,0,HOMEVAL+dx+offset);
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
