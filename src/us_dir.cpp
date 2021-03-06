/**/

#include <wiringPi.h>
#include <stdio.h>
#include "PCA9685.h"
#include "us_dir.h"

static PCA9685 * pwm;

void usdirInit(void){
	pwm = new PCA9685(1, 0x40);
	pwm->setPWMFreq(90);
}

void seeHome(){
	pwm->setPWM(CH14, 0, HOME_PULSE);
}

void seeLeft(int angle){
	pwm->setPWM(CH14, 0, HOME_PULSE + (int)(angle/90.0*MAX_DIFF));
}

void seeRight(int angle){
	pwm->setPWM(CH14, 0, HOME_PULSE - (int)(angle/90.0*MAX_DIFF));
}

void usdirTest(void){
	seeHome();
	delay(1000);
	seeLeft(45);
	delay(1000);
	seeRight(45);
	delay(1000);
	seeHome();
	delay(1000);
}
void usdirTerm(void)
{
	seeHome();
	pwm->setPWM(CH14, 0, 0); //servo moter power off
	delete pwm;
}
