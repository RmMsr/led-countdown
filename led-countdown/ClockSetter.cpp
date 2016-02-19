#include "ClockSetter.h"
#include <Sodaq_DS3231.h>

void ClockSetter::begin()
{
  Serial.println("Console Input sets DS3231 clock");
  printCurrentTime();
  reset();
  nextPart();
}

void ClockSetter::proccess()
{
  if ( Serial.available() && currentPart < 7 ) {
    parts[currentPart] = Serial.parseInt();
    currentPart++;
  } else if ( currentPart >= 7 ) {
    while ( Serial.available() ) {
      Serial.read();
    }
    setDate(parts);
    printCurrentTime();
    reset();
  }
}

void ClockSetter::reset()
{
  for ( uint8_t i = 0; i < 7; i++ ) {
    parts[i] = 0;
  }
  currentPart = 0;
}

void ClockSetter::nextPart()
{
  Serial.println("Specify the new date in UTC");
  Serial.println("year month day hour minute second weekday");
  Serial.println("Wekdays are 0: sun, 1: mon, 2: tue, 3: wed, 4: thu, 5: fri, 6: sat");
  Serial.println("Enter from part: " + String(currentPart));
}

void ClockSetter::setDate(uint16_t dateData[7])
{
  Serial.println(String("Setting date: ") 
    + dateData[0] + "-"+ dateData[1] + "-" + dateData[2] + "T"
    + dateData[3] + ":"+ dateData[4] + ":" + dateData[5] + "Z"
    + " day " + dateData[6]
  );
  DateTime time(
    dateData[0], 
    dateData[1], 
    dateData[2], 
    dateData[3], 
    dateData[4], 
    dateData[5], 
    dateData[6]);
  rtc.setDateTime(time);
}

void ClockSetter::printCurrentTime()
{
  String out("Current time: ");
  DateTime dateTime = rtc.now();
  dateTime.addToString(out);
  switch (dateTime.dayOfWeek()) {
    case 0: out += " UTC Sunday"; break;
    case 1: out += " UTC Monday"; break;
    case 2: out += " UTC Tuesday"; break;
    case 3: out += " UTC Wednesday"; break;
    case 4: out += " UTC Thursday"; break;
    case 5: out += " UTC Friday"; break;
    case 6: out += " UTC Saturday"; break;
  }
  Serial.println(out);
}

