#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "ultrasonic.h"
#include "lanekeeping.h"
#include "camera.h"

using namespace std;
using namespace cv;

volatile int usflag=0;
int dist;

void *ultrasonicDetection(void * param){
	printf("US detection thread has been created\n");
	while(true){
		dist = getCM();
		printf("US DIST : %d\n", dist);
		//AEB
		if(usflag==0&&dist<=40)
		{
			usflag=1;
		}
		//Switch to normal driving
		else if(usflag&&dist>40)
		{
			usflag=0;
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
	//Send trig pulse
	digitalWrite(TRIG, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIG, HIGH);
	delayMicroseconds(20);
	digitalWrite(TRIG, LOW);

	//Wait for echo start
	while(digitalRead(ECHO) == LOW);
	//Wait for echo end
	long startTime = micros();
	while(digitalRead(ECHO) == HIGH);
	long travelTime = micros() - startTime;

	//Get distance in cm
	int distance = travelTime / 58;

	return distance;
}

void usTerm() {
	digitalWrite(TRIG, LOW);
}
