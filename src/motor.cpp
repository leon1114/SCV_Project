/**/

#include <wiringPi.h>
#include <stdio.h>
#include "motor.h"

static PCA9685 * pwm;
static int currSpeed;
unsigned int pins[] = {Motor0_A, Motor0_B, Motor1_A, Motor1_B};
bool forward0 = true, forward1 = true, backward0 = false, backward1 = false;

int getSpeed(){
	return currSpeed;
}

void setSpeed(int speed){
	currSpeed = speed;
	speed *= 40;
	pwm->setPWM(EN_M0, 0, speed);
	pwm->setPWM(EN_M1, 0, speed);
}

void motorInit(void){
	unsigned int i;
	pwm = new PCA9685(1, 0x40);
	pwm->setPWMFreq(60);
	forward0 = true;
	forward1 = true;
	wiringPiSetupPhys();
	//pwmSetMode(WPI_MODE_PHYS);

	backward0 = forward0==true?false:true;
	backward1 = forward1==true?false:true;

	for(i=0; i<sizeof(pins); i++)
		pinMode(pins[i], OUTPUT);

	setSpeed(10);
}

void motor0(int in){
	if(in == true){
		digitalWrite(Motor0_A, LOW);
		digitalWrite(Motor0_B, HIGH);
	}else{
		digitalWrite(Motor0_A, HIGH);
		digitalWrite(Motor0_B, LOW);
	}
}

void motor1(int in){
	if(in == true){
		digitalWrite(Motor1_A, LOW);
		digitalWrite(Motor1_B, HIGH);
	}else{
		digitalWrite(Motor1_A, HIGH);
		digitalWrite(Motor1_B, LOW);
	}
}

void forward(){
	motor0(forward0);
	motor1(forward1);
}

void backward(){
	motor0(backward0);
	motor1(backward1);
}

void forwardWithSpeed(int spd = 50){
	setSpeed(spd);
	motor0(forward0);
	motor1(forward1);
}

void backwardWithSpeed(int spd = 50){
	setSpeed(spd);
	motor0(backward0);
	motor1(backward1);
}

void stop(){
	unsigned int i;
	for(i = 0; i < sizeof(pins); i++){
		digitalWrite(pins[i], LOW);
	}
}

void ctrl(int status, int dir = 1){
	if(status == 1){
		if(dir == 1)
			forward();
		else
			backward();
	}
	else if(status == 0)
		stop();
	else
		printf("Argument error! status must be 0 or 1\n");
}

void motorTest(){
	int i;
	motorInit();
	printf("motor-started!\n");
	for(i=0; i<3; i++){
		ctrl(1);
		delay(3000);
		setSpeed(10);
		delay(3000);
		setSpeed(100);
		ctrl(0);
		delay(2000);
	}
	printf("motor-ended!\n");
}

void motorTerm(void)
{
	delete pwm;
}
