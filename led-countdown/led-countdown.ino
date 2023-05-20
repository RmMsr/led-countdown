#include <Adafruit_NeoPixel.h>
#include <Sodaq_DS3231.h>
#include "TimeTable.h"
#include "ClockSetter.h"

const long serialRate = 9600;
const int inputPin = 8;
const int pixelPin = 3;
const int loopDelay = 1;
const unsigned long millisPerNumber = (unsigned long)1000 * 60;  // How long is one led shown?

const uint8_t glowMinBrightness = 30;
const uint8_t glowMaxBrightness = 60;
const uint8_t glowSeconds = 8;

bool isRunning = false;
bool lastState = false;
int number = 0;
unsigned long buttonMillis = 0;
unsigned long endMillis = 0;
unsigned long animationStart = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, pixelPin, NEO_GRB + NEO_KHZ800);
const uint32_t color1 = strip.Color(255, 0, 195);

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
    if (not isRunning) {
      idleAnimation();
    }
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

void proccessState(bool pressed) {
  if (lastState != pressed) {
    startMeasure();
  } else {
    applyMeasure();
  }

  if (isRunning == false) {
    isRunning = true;
    log("Manually started countdown");
  }
}

void startMeasure() {
  buttonMillis = millis();
  setNumber(0);
}

void applyMeasure() {
  unsigned long duration = millis() - buttonMillis;

  int newNumber = duration / 100;
  if (newNumber > strip.numPixels()) {
    newNumber = strip.numPixels();
  } else if (newNumber < 0) {
    newNumber = 0;
  }

  startCountdownMinutes(newNumber);
}

void proccessTime() {
  if (isRunning == false) {
    return;
  }

  if (endMillis > millis()) {
    int remainingNumber = (int)((endMillis - millis()) / millisPerNumber);
    setNumber(remainingNumber);
  } else {
    setNumber(0);
    isRunning = false;
    log("Stopped countdown\n");
  }
}

void setNumber(int num) {
  if (num < 0 || num > strip.numPixels()) {
    return;
  }
  number = num;
  showNumber((uint16_t)number, color1);
}

void showNumber(uint16_t num, uint32_t color) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    if (i < num) {
      strip.setPixelColor(i, color);
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
  }

  strip.setBrightness(animationMinMax(
    animationCurvedAmplitude(glowSeconds, animationStart), glowMinBrightness, glowMaxBrightness)
  );

  strip.show();
}

void checkTimeTable() {
  TimeTable table(rtc.now());
  if (table.match()) {
    startCountdownMinutes(60);
    log("TimeTable started Countdown");
  }
}

void startCountdownMinutes(unsigned char minutes) {
  endMillis = millis() + (minutes + 1) * millisPerNumber - 1;
  isRunning = true;
  animationStart = millis();
  setNumber(minutes);
}

void log(const String& message) {
  String output = TimeTable::printDateTime(TimeTable::applyTimeZone(rtc.now()));
  Serial.println(output + " " + message);
}

void idleAnimation() {
  if (animationStart == 0) {
    Serial.println("Animating...");
    animationStart = millis();
  }

  const uint16_t outline[] = {
    1, 2, 3, 4, 5, 6,
    11, 12, 13, 18, 19,
    20, 28, 29,
    30, 31, 39,
    40, 41, 46, 47, 48,
    53, 54, 55, 56, 57, 58
  };

  const uint16_t fill[] = {
    14, 15, 16, 17,
    21, 22, 23, 24, 25, 26, 27,
    32, 33, 34, 35, 36, 37, 38,
    42, 43, 44, 45
  };

  const uint32_t outlineColour = strip.Color(255, 90, 0);
  const uint32_t fillColour = strip.Color(240, 140, 00);

  for (int i = 0; i < (sizeof(outline) / sizeof(uint16_t)); i++) {
    strip.setPixelColor(outline[i], outlineColour);
  }

  for (int i = 0; i < (sizeof(fill) / sizeof(uint16_t)); i++) {
    strip.setPixelColor(fill[i], fillColour);
  }

  strip.setBrightness(animationMinMax(
    animationCurvedAmplitude(glowSeconds, animationStart), glowMinBrightness, glowMaxBrightness)
  );

  strip.show();
}

unsigned char animationMinMax(double amplitude, unsigned char min, unsigned char max) {
  return floor(amplitude * (max - min) + min);
}

double animationCurvedAmplitude(unsigned char durationSeconds, unsigned long startMillis) {
  // Returns 0..1
  return (sin(animationAmplitude(durationSeconds, startMillis) * 6.283) + 1) / 2;
}

double animationAmplitude(unsigned char durationSeconds, unsigned long startMillis) {
  // Returns 0..1
  return double((millis() - startMillis) % (durationSeconds * 1000)) / (durationSeconds * 1000);
}
