#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "ultrasonic.h"
#include "lanekeeping.h"
#include "camera.h"
#include "us_dir.h"

using namespace std;
using namespace cv;

volatile int usflag=0;
int dist;
int angle;

void *ultrasonicDetection(void * param){

	printf("Ultrasonic detection thread has been created\n");
	int ret;
	int offset = 0, dir = 5;
	while(true){
		if(angle == 0 && usflag == 0){
			offset+=dir;
			if(offset>=10 || offset<=-10) dir*=-1;
			seeAngle(angle + offset);
		}else seeAngle(angle);
		ret = getCM();
		if(ret == -1) continue;
		dist = ret;
//		printf("dist : %d \n", dist);
		//AEB
		if(usflag==0&&dist<=40)
		{
			usflag=1;
			printf("US flag on dist: %d\n", dist);

		}
		//Switch to normal driving
		else if(usflag == 1 && dist>40)
		{
			usflag=0;
			printf("US flag off dist: %d\n", dist);
		}
		delay(100);
	}
	return NULL;
}

void usInit() {

	pinMode(TRIG, OUTPUT);
	pinMode(ECHO, INPUT);

	//TRIG pin must start LOW
	digitalWrite(TRIG, LOW);
	delay(30);
}

int getCM() {
	int i;
	//Send trig pulse
	digitalWrite(TRIG, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIG, HIGH);
	delayMicroseconds(20);
	digitalWrite(TRIG, LOW);

	//Wait for echo start
	for(i=0; i<500000;i++){
		if(digitalRead(ECHO) != LOW)
			break;
	}
	//Wait for echo end
	long startTime = micros();
	for(i=0; i<500000;i++){
		if(digitalRead(ECHO) != HIGH)
			break;
	}
	long travelTime = micros() - startTime;

	//Get distance in cm
	int distance = travelTime / 58;
	if(distance>1000) return -1;
	return distance;
}

void usTerm() {
	digitalWrite(TRIG, LOW);
}
