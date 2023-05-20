#include "TimeTable.h"

TimeTable::TimeTable(const DateTime& dateTimeUtc)
{
  this->dateTime = applyTimeZone(dateTimeUtc);
}

bool TimeTable::match() const
{
  switch ( this->dateTime.dayOfWeek() ) {
    case MON:
    case TUE:
    case WED:
    case THU:
    case FRI:
      return this->dateTime.hour() == 7 && this->dateTime.minute() == 15;
  }
}

DateTime TimeTable::applyTimeZone(const DateTime& dateTimeUtc)
{
  uint16_t year = dateTimeUtc.year();
  uint8_t month = dateTimeUtc.month();
  uint8_t day = dateTimeUtc.date();
  uint8_t hour = dateTimeUtc.hour();
  uint8_t minute = dateTimeUtc.minute();
  uint8_t second = dateTimeUtc.second();
  uint8_t weekday = dateTimeUtc.dayOfWeek();

  // CET, CEST
  bool isLastSundayFirstHour = day > 24 && weekday == SUN && hour < 1;
  bool isPastLastSunday1AM   = day - weekday >= 24 && !isLastSundayFirstHour;
  bool isBeforeLastSunday1AM = day - weekday <  24 ||  isLastSundayFirstHour;
  if (
    ( month > 3 || month == 3 && isPastLastSunday1AM ) && 
    ( month < 10 || month == 10 && isBeforeLastSunday1AM )
   ) {
    // Summer time
    hour += 2;
  } else {
    // Winter time
    hour += 1;
  }

  DateTime newTime(year, month, day, hour, minute, second, weekday);
  return newTime;
}

String TimeTable::printDateTime(const DateTime& dateTime)
{
  String output;
  dateTime.addToString(output);
  switch (dateTime.dayOfWeek()) {
    case 0: output += " (Sunday)"; break;
    case 1: output += " (Monday)"; break;
    case 2: output += " (Tuesday)"; break;
    case 3: output += " (Wednesday)"; break;
    case 4: output += " (Thursday)"; break;
    case 5: output += " (Friday)"; break;
    case 6: output += " (Saturday)"; break;
  }
  return output;
}

