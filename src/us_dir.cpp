/**/

#include <wiringPi.h>
#include <stdio.h>
#include "PCA9685.h"
#include "us_dir.h"

static PCA9685 * pwm;

void usdirInit(void){
	pwm = new PCA9685(1, 0x40);
	pwm->setPWMFreq(120);
}

void seeHome(){
	pwm->setPWM(CH14, 0, HOME_PULSE);
}

void seeLeft(int angle){
	pwm->setPWM(CH14, 0, HOME_PULSE + angle/90*MAX_DIFF);
}

void seeRight(int angle){
	pwm->setPWM(CH14, 0, HOME_PULSE - angle/90*MAX_DIFF);
}

void usdirTerm(void)
{
	seeHome();
	pwm->setPWM(CH14, 0, 0); //servo moter power off
	delete pwm;
}
