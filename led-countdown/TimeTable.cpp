#include "TimeTable.h"

TimeTable::TimeTable(const DateTime& dateTimeUtc)
{
  this->dateTime = applyTimeZone(dateTimeUtc);
}

bool TimeTable::match() const
{
  switch ( this->dateTime.dayOfWeek() ) {
    case MON:
      return this->dateTime.hour() == 8 && this->dateTime.minute() == 0;
    case TUE:
      return this->dateTime.hour() == 8 && this->dateTime.minute() == 30;
    case WED:
    case THU:
      return this->dateTime.hour() == 7 && this->dateTime.minute() == 00;    
    case FRI:
      return this->dateTime.hour() == 16 && this->dateTime.minute() == 40;
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

