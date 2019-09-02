#include "sensordisplay.h"



sensordisplay::sensordisplay() :
	mx(MD_MAX72XX(MD_MAX72XX::GENERIC_HW, LED_CS_PIN, 1))
{
}

sensordisplay::~sensordisplay()
{
}

void sensordisplay::setup()
{
	mx.begin();
	mx.clear();
}

void sensordisplay::writeNumber(int8_t dist)
{
	
	if (dist >= 0 && dist < IMAGES_LEN)
	{
		for (byte row = 0; row < 8; row++)
		{
			mx.setRow(0, row, IMAGES[dist][row]);
		}
	}
}

void sensordisplay::setStopDistance(int8_t dist)
{
	mStopDistance = dist;
}

int8_t sensordisplay::getStopDistance()
{
	return mStopDistance;
}

bool sensordisplay::isInSensorRange(int8_t currentDistance)
{
	return (currentDistance >= 0 && currentDistance <= MAX_DISPLAY_DISTANCE);
}

bool sensordisplay::isGreaterThanStopDistance(int8_t currentDistance)
{
	return (currentDistance > (mStopDistance + DIST_BUFFER));
}

bool sensordisplay::isLessThanStopDistance(int8_t currentDistance)
{
	return (currentDistance < (mStopDistance - DIST_BUFFER));
}

bool sensordisplay::isInStopDistance(int8_t currentDistance)
{
	return (currentDistance <= (mStopDistance + DIST_BUFFER) &&
		currentDistance >= (mStopDistance - DIST_BUFFER));
}

void sensordisplay::guide(int8_t newDistance)
{
	mCurrentDistance = newDistance;

	if (!isInSensorRange(newDistance))
	{
		mx.clear();
		return;
	}

	if (isGreaterThanStopDistance(newDistance))
	{
		showForward();
	}
	else if (isLessThanStopDistance(newDistance))
	{
		showReverse();
	}
	else
	{
		showStop();
	}
}

void sensordisplay::showActivity()
{
	unsigned long m = millis();
	unsigned long diff = m - mActivityLedMillis;

	if (diff > mActivityLedDelay)
	{
		mActivityLedMillis = m;
		mActivityLed = !mActivityLed;
	}

	if (!isInSensorRange(mCurrentDistance))
	{
		mActivityLedDelay = 1000;
		mx.setPoint(0, 0, mActivityLed);
	}
	else if (isGreaterThanStopDistance(mCurrentDistance))
	{
		mActivityLedDelay = 250;
		mx.setPoint(0, 0, mActivityLed);
		mx.setPoint(0, 7, mActivityLed);
	}
	else if (isLessThanStopDistance(mCurrentDistance))
	{
		mActivityLedDelay = 100;
		mx.setPoint(7, 0, mActivityLed);
		mx.setPoint(7, 7, mActivityLed);
	}

}

int8_t sensordisplay::getDistance()
{
	return mCurrentDistance;
}

void sensordisplay::showForward()
{
	for (byte row = 0; row < 8; row++)
	{
		mx.setRow(0, row, IMAGES[0][row]);
	}
}

void sensordisplay::showReverse()
{
	for (byte row = 0; row < 8; row++)
	{
		mx.setRow(0, row, IMAGES[1][row]);
	}
}

void sensordisplay::showStop()
{
	for (byte row = 0; row < 8; row++)
	{
		mx.setRow(0, row, IMAGES[2][row]);
	}
}

