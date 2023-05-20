#include <Arduino.h>
#include "SerialInterface.h"

#ifndef CLOCK_SETTER_H
#define CLOCK_SETTER_H

class ClockSetter : public SerialInterface
{
	public:
	void begin();
  void proccess();

	private:
	void reset();
	void nextPart();
	void setDate(uint16_t dateData[7]);
	void printCurrentTime();

	uint16_t parts[7]; // year, month, day, hour, minute, second, weekday
	uint8_t currentPart = 0;
};

#endif
