#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_VL53L0X.h>

#define CEPIN 9
#define CSPIN 10

RF24 radio(CEPIN, CSPIN);
byte addresses[][6] = { "1Distance" };


Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setupRadio()
{
	Serial.println("Setup radio");
	radio.begin();
	radio.setRetries(15, 15);
	radio.setPALevel(RF24_PA_MIN);
	radio.openWritingPipe(addresses[0]);
	radio.stopListening();
}

void setupVL52L0X()
{
	Serial.println("Setup VL53L0X");
	if (!lox.begin())
	{
		Serial.println(F("Failed to boot VL53L0X"));
		while (1);
	}
}

void setup()
{
	Serial.begin(115200);
	// wait until serial port opens for native USB devices
	while (!Serial)
	{
		delay(1);
	}

	setupRadio();
	setupVL52L0X();
}

#define NUM_MEAS 10
int8_t distances[NUM_MEAS];
byte distanceIndex = 0;
unsigned long prevSendMillis = 0;

void sendDistance()
{
	unsigned long m = millis();
	if (m - prevSendMillis > 100)
	{
		long s = 0;
		int8_t distance = -1;
		for (int i = 0; i < NUM_MEAS - 1; i++)
		{
			s += distances[i];
			int8_t diff = abs(distances[i] - distances[i + 1]);

			if (diff > 30)
			{
				distance = -2;
				radio.write((int8_t*)&distance, sizeof(int8_t));
				return;
			}
		}

		s += distances[NUM_MEAS - 1];
		distance = s / NUM_MEAS;
		radio.write((int8_t*)&distance, sizeof(int8_t));
		prevSendMillis = m;
	}
}

void loop()
{
	VL53L0X_RangingMeasurementData_t measure;
	lox.rangingTest(&measure, false);

	// phase failures have incorrect data
	if (measure.RangeStatus != 4)
	{
		distances[distanceIndex] = measure.RangeMilliMeter / 10.0;
	}
	else
	{
		distances[distanceIndex] = -1;
	}

	distanceIndex++;
	if (distanceIndex >= NUM_MEAS)
	{
		distanceIndex = 0;
	}

	sendDistance();
	delay(10);
}
