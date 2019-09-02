#pragma once

#include <MD_MAX72xx.h>
#include "images.h"

#define LED_ADDR 0
#define LED_CS_PIN 8

// maximum 200 cm
#define MAX_DISPLAY_DISTANCE 200
// +/- 10 cm = 20 cm buffer for stopping
#define DIST_BUFFER 10


const byte ALL_ON[][8] = {
	{
		B11111111,
		B11111111,
		B11111111,
		B11111111,
		B11111111,
		B11111111,
		B11111111,
		B11111111
	} };

const byte STEADY_ON[][8] = {
	{
		B10000001,
		B00000000,
		B00000000,
		B00000000,
		B00000000,
		B00000000,
		B00000000,
		B10000001
	} };

const uint8_t arrow[8] =
{
  0b00001000,
  0b00011100,
  0b00111110,
  0b01111111,
  0b00011100,
  0b00011100,
  0b00111110,
  0b00000000
};

class sensordisplay
{
public:
	sensordisplay();
	~sensordisplay();

	void setup();
	void writeNumber(int8_t dist);
	void setStopDistance(int8_t dist);
	int8_t getStopDistance();
	
	void guide(int8_t currentDistance);
	int8_t getDistance();
	void showActivity();

private:
	MD_MAX72XX mx;
	int8_t mStopDistance = 25;
	int8_t mCurrentDistance = 0;

	bool isInSensorRange(int8_t currentDistance);

	bool isGreaterThanStopDistance(int8_t currentDistance);

	bool isLessThanStopDistance(int8_t currentDistance);

	bool isInStopDistance(int8_t currentDistance);

	

	void showForward();
	void showReverse();
	void showStop();
	int8_t mPrevDistance = -1;
	bool mActivityLed = false;
	unsigned long mActivityLedMillis = 0;
	unsigned long mActivityLedDelay = 250;
};

