#include <stdio.h>
#include <wiringPi.h>

#define trigPin 5
#define echoPin 4

int main(void)
{
	if (wiringPiSetup() == -1) return 1;

	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);

	for (;;)
	{
		digitalWrite(trigPin, LOW);
		delayMicroseconds(2);
		digitalWrite(trigPin, HIGH);
		delayMicroseconds(20);
		digitalWrite(trigPin, LOW);

		while(digitalRead(echoPin) == LOW);

		long startTime = micros();
		while(digitalRead(echoPin) == HIGH);
		long travelTime = micros() - startTime;

		int distance = travelTime / 58;

		printf("Distance : %dcm\n", distance);
		delay(100);
	}
	return 0;
}

