#include <RotaryEncoder.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

#include <eeprom.h>


#include "sensordisplay.h"

#define NRF24L01_CE_PIN 9
#define NRF24L01_CS_PIN 10

RF24 radio(NRF24L01_CE_PIN, NRF24L01_CS_PIN);
byte addresses[][6] = { "1Distance" };

#define OLED_I2C_ADDRESS 0x3C
SSD1306AsciiWire display;

sensordisplay sd;

#define ROTARY_CLK_PIN 4
#define ROTARY_DT_PIN 5
#define ROTARY_SW_PIN 2

RotaryEncoder rt(ROTARY_DT_PIN, ROTARY_CLK_PIN);
volatile bool showSaved = false;

void saveStopDistance()
{
	// started getting segmentation fault during compile after adding noInterrupts()/interrupts()
	noInterrupts();
	EEPROM.write(0, sd.getStopDistance());
	showSaved = true;
	interrupts();
}

void restoreStopDistance()
{
	showSaved = false;
	uint8_t stop = EEPROM.read(0);
	sd.setStopDistance(stop);
	Serial.print("Restored stop distance: ");
	Serial.println(stop);
}

void setupOLED()
{
	display.begin(&Adafruit128x32, OLED_I2C_ADDRESS);
	display.set400kHz();
	display.setFont(Adafruit5x7);
	display.clear();
	display.setCursor(0, 0);
	display.print(F("Sensor display"));
}

void setupRadio()
{
	radio.begin();
	radio.setPALevel(RF24_PA_MAX);
	radio.setAutoAck(1);
	radio.setRetries(15, 15);
	radio.openReadingPipe(0, addresses[0]);
	radio.startListening();
}

void setup()
{
    Serial.begin(115200);   
    Serial.println(F("Sensor display"));
	
	sd.setup();
	restoreStopDistance();

	setupRadio();
	setupOLED();

	displayStopDistance(sd.getStopDistance());
	displayDistance(sd.getDistance());

	pinMode(ROTARY_SW_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(ROTARY_SW_PIN), saveStopDistance, FALLING);
}

void displayStopDistance(int8_t stopDistance)
{
	display.setCursor(0, 3);
	display.clearToEOL();
	display.print(F("Stop: "));
	display.print(stopDistance);
	display.print(F(" cm"));
	if (showSaved)
	{
		display.print(F(" *"));
		showSaved = false;
	}
}

void displayDistance(int8_t distance)
{
	display.setCursor(0, 2);
	display.clearToEOL();
	display.print(F("Distance: "));
	display.print(distance);
	display.print(F(" cm"));
}

void checkStopDistance()
{
	int8_t stop = sd.getStopDistance();

	rt.tick();
	int8_t dir = rt.getDirection();
	if (dir != 0)
	{
		stop += dir;
		if (stop < 15)
		{
			stop = 15;
		}
		else if (stop > 100)
		{
			stop = 100;
		}

		sd.setStopDistance(stop);
		displayStopDistance(stop);
	}
	else if (showSaved)
	{
		displayStopDistance(stop);
		Serial.print("Saved stop distance to EEPROM: ");
		Serial.println(stop);
	}
}

void loop()
{      
    if (radio.available())
    {      
      int8_t newDistance;
	  int8_t currentDistance = sd.getDistance();

      radio.read((int8_t*)&newDistance, sizeof(int8_t));
	  sd.guide(newDistance);

	  if (newDistance != currentDistance)
	  {
		  displayDistance(newDistance);
	  }
    }

	checkStopDistance();
	sd.showActivity();
}
