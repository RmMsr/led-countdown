#include <Adafruit_NeoPixel.h>
#include <Sodaq_DS3231.h>
#include "TimeTable.h"
#include "ClockSetter.h"

const long serialRate = 9600;
const int inputPin = 8;
const int pixelPin = 3;
const int loopDelay = 100;
const unsigned long millisPerNumber = (unsigned long) 1000 * 60; // How long is one led shown?

bool isRunning = false;
bool lastState = false;
int number = 0;
unsigned long lastMillis = 0;
unsigned long buttonMillis = 0;
unsigned long endMillis = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, pixelPin, NEO_GRB + NEO_KHZ800);
const uint32_t color1 = strip.Color(30, 0, 30);

SerialInterface* console = NULL;

void setup() {
  // Console
  Serial.begin(serialRate);

  //Clock
  rtc.begin();

  // Button
  pinMode(inputPin, INPUT);
  digitalWrite(inputPin, HIGH);
  
  strip.begin();
  setNumber(number);
  
  isRunning = false;
  lastState = false;
  lastMillis = millis();
  endMillis = millis();

  log("Let's go");
  console = new ClockSetter();
  console->begin();
}

void loop() {
  int lastNumber = number;
  
  bool buttonPressed = digitalRead(inputPin) == LOW;

  if (buttonPressed) {
    proccessState(buttonPressed);
  } else if (isRunning) {
    proccessTime();
  } else {
    checkTimeTable();
  }
  lastState = buttonPressed;

  // Logging
  if (lastNumber != number) {
    log(String("Set Number to ") + number);
  }

  // Console
  console->proccess();

  delay(loopDelay);
}

void proccessState(bool pressed)
{
  if (lastState != pressed) {
    startMeasure();
  } else {
    applyMeasure();
  }
  
  if ( isRunning == false ) {
    isRunning = true;
    log("Manually started countdown");
  }
}

void startMeasure()
{
  buttonMillis = millis();
  setNumber(0);
}

void applyMeasure()
{
  unsigned long duration = millis() - buttonMillis;
  
  int newNumber = duration / 100;
  if ( newNumber > strip.numPixels() ) {
    newNumber = strip.numPixels();
  } else if ( newNumber < 0 ) {
    newNumber = 0;
  }

  startCountdownMinutes(newNumber);
}

void proccessTime()
{
  if ( isRunning == false ) {
    return;
  }
  
  if ( endMillis > millis() ) {
    int remainingNumber = (int) ((endMillis - millis()) / millisPerNumber);
    setNumber(remainingNumber);
  } else {
    setNumber(0);
    isRunning = false;
    log("Stopped countdown\n");
  }
}

void setNumber(int num)
{
  if ( num < 0 || num > strip.numPixels() ) {
    return;
  }
  number = num;
  showNumber((uint16_t) number, color1);
}

void showNumber(uint16_t num, uint32_t color)
{  
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    if (i < num) {
      strip.setPixelColor(i, color);
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
  }
  strip.show();
}

void checkTimeTable()
{
  TimeTable table(rtc.now());
  if ( table.match() ) {
    startCountdownMinutes(60);
    log("TimeTable started Countdown");
  }
}

void startCountdownMinutes(unsigned char minutes)
{
  endMillis = millis() + (minutes + 1) * millisPerNumber - 1;
  isRunning = true;
  setNumber(minutes);
}

void log(const String& message)
{
  String output = TimeTable::printDateTime(TimeTable::applyTimeZone(rtc.now()));
  Serial.println(output + " " + message);
}

