#include <Arduino.h>
#include <Sodaq_DS3231.h>

class TimeTable
{
  public:
  enum WEEK_DAYS { SUN=0, MON=1, TUE=2, WED=3, THU=4, FRI=5, SAT=6 };

  TimeTable(const DateTime& dateTime);
  bool match() const;
  const DateTime& getLocal() { return this->dateTime; }
  static DateTime applyTimeZone(const DateTime& dateTimeUtc);
  static String printDateTime(const DateTime& dateTime);

  private:
  DateTime dateTime;
};

