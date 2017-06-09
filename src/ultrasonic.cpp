#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "ultrasonic.h"
#include "lanekeeping.h"
#include "camera.h"

using namespace std;
using namespace cv;

volatile int usflag=0;
extern Mat img;

void *ultrasonicDetection(void * param){
	int dist;
	printf("ultrasonic detection part");
	while(true){
		dist = getCM();
		//AEB
		if(usflag==0&&dist<=10){
			/*** Image analysis ***/


			usflag=1;
		}
		//Switch to normal driving
		else if(usflag&&dist>10){
			usflag=0;
		}
	}
	return NULL;
}

void usInit() {
	wiringPiSetup();
	pinMode(TRIG, OUTPUT);
	pinMode(ECHO, INPUT);

	//TRIG pin must start LOW
	digitalWrite(TRIG, LOW);
	delay(30);
}

int getCM() {
	//Send trig pulse
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
	//TRIG pin must start LOW
	digitalWrite(TRIG, LOW);
}
