#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "ultrasonic.h"

volatile int usflag=0;

void *ultrasonicDetection(void * param){
	int dist;
	printf("ultrasonic detection part");
	while(true){
		dist = getCM();
		if(usflag==0&&dist<=10){
			/*** 이미지 분석하는 코드 필요 ***/
			usflag=1;
		}
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
