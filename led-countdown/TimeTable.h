#include <Arduino.h>
#include <Sodaq_DS3231.h>

class TimeTable
{
  public:
  enum WEEK_DAYS { SUN=1, MON=2, TUE=3, WED=4, THU=5, FRI=6, SAT=7 };

  TimeTable(const DateTime& dateTime);
  bool match() const;
  const DateTime& getLocal() { return this->dateTime; }

  private:
  static DateTime applyTimeZone(const DateTime& dateTimeUtc);
  DateTime dateTime;
};

