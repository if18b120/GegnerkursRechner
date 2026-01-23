#include "LedControl.h"


#define DISPLAY_COUNT 5
#define MSTOKTCONVERSION 1.94384
#define SAVECOUNT 10

LedControl lc = LedControl(10, 12, 11, DISPLAY_COUNT);

enum Mode {
  LEFT,
  RIGHT
};

struct State {
  int length = 100;
  int height = 20;
  int opticalLength = 0;
  int opticalHeight = 0;
  int time = 0;
};

volatile int brightness = 0;
volatile bool magnified = false;
State saves[SAVECOUNT];
volatile State* currentState = &(saves[0]);

volatile int save = 0;
Mode modes[3] = { RIGHT, RIGHT, RIGHT };


volatile unsigned long debounce = 0;

float calculateDistance(float height, float opticalHeight, bool magnified) {
  if (opticalHeight <= 0) {
    return 999;
  }
  if (magnified) {
    opticalHeight /= 4;  // Correct the angular size
  }
  return height / tan(radians(opticalHeight / 16));
}

float calculateAngle(float distance, float length, float opticalLength, bool magnified) {
  if (length <= 0) {
    return 0;
  }
  if (magnified) {
    opticalLength /= 4;  // Correct the angular size
  }
  float apparentWidth = 2 * distance * tan(radians(opticalLength) / 2);
  float AoB = degrees(acos(apparentWidth / length));
  return AoB;
}

float calculateSpeed(float length, float time) {
  if (time <= 0) return 999;
  return length / time * MSTOKTCONVERSION;
}

void writeNumberToDisplay(int display, int value, bool leftBound) {
  writeNumberToDisplay(display, value, leftBound, false);
}

void writeNumberToDisplay(int display, int value, bool leftBound, bool dot) {
  if (value > 999) value = 999;
  if (value < 0) value = 0;

  int index = 0;
  int offset = 0;
  if (leftBound) {
    offset = 5;
  }

  for (int i = 0; i < 3; i++) {
    lc.setChar(display, offset + i, ' ', false);
  }

  do {
    int digit = value % 10;
    value = value / 10;
    lc.setDigit(display, offset + index, digit, index == 0 && dot);
    index++;
  } while (value > 0);
}

void writeFloatToDisplay(int display, float value, bool leftBound) {
  if (value > 99.9) value = 99.9;
  if (value < 0) value = 0;
  int num = value * 10;

  int index = 0;
  int offset = 0;
  if (leftBound) {
    offset = 5;
  }

  for (int i = 0; i < 3; i++) {
    lc.setChar(display, offset + i, ' ', false);
  }

  do {
    int digit = num % 10;
    num /= 10;
    lc.setDigit(display, offset + index, digit, index == 1);
    index++;
  } while (num > 0);
}

ISR(PCINT0_vect) {
  checkPins();
}

ISR(PCINT1_vect) {
  checkPins();
}

ISR(PCINT2_vect) {
  checkPins();
}

void checkPins() {
  unsigned long now = millis();

  if (now - debounce < 40) {
    return;
  }
  debounce = now;

  // switch A0, A1
  if (bitRead(PINC, 0) == 0) {
    magnified = true;
  } else if (bitRead(PINC, 1) == 0) {
    magnified = false;
  }

  // encoder brightness D0, D1
  if (bitRead(PIND, 0) == 0) {
    if (bitRead(PIND, 1) == 0) {
      if (brightness < 15) brightness++;
    } else {
      if (brightness > 0) brightness--;
    }
  }

  // encoder length D7, D8, D9
  if (bitRead(PIND, 7) == 0) {
    int* value;
    if (modes[0] == LEFT) {
      value = &currentState->length;
    } else {
      value = &currentState->opticalLength;
    }
    if (bitRead(PINB, 0) == 0) {
      if ((*value) > 0) (*value)--;
    } else {
      if ((*value) < 999) (*value)++;
    }
  }
  if (bitRead(PINB, 1) == 0) {
    if (modes[0] == LEFT) {
      modes[0] = RIGHT;
    } else {
      modes[0] = LEFT;
    }
  }

  // encoder height D4, D5, D6
  if (bitRead(PIND, 4) == 0) {
    int* value;
    if (modes[1] == LEFT) {
      value = &currentState->height;
    } else {
      value = &currentState->opticalHeight;
    }
    if (bitRead(PIND, 5) == 0) {
      if ((*value) > 0) (*value)--;
    } else {
      if ((*value) < 999) (*value)++;
    }
  }
  if (bitRead(PIND, 6) == 0) {
    if (modes[1] == LEFT) {
      modes[1] = RIGHT;
    } else {
      modes[1] = LEFT;
    }
  }

  // encoder time/save A3, A4, A5
  if (modes[2] == LEFT) {
    if (bitRead(PINC, 5) == 0) {
      if (bitRead(PINC, 4) == 0) {
        if (save > 0) save--;
      } else {
        if (save < SAVECOUNT) save++;
      }
      currentState = &(saves[save]);
    }
  } else {
    if (bitRead(PINC, 5) == 0) {
      if (bitRead(PINC, 4) == 0) {
        if (currentState->time > 0) currentState->time--;
      } else {
        if (currentState->time < 999) currentState->time++;
      }
    }
  }

  if (bitRead(PINC, 3) == 0) {
    if (modes[2] == LEFT) {
      modes[2] = RIGHT;
    } else {
      modes[2] = LEFT;
    }
  }
}

void initScreen() {
  writeFloatToDisplay(0, 0, true);
  writeFloatToDisplay(0, 0, false);
  writeFloatToDisplay(1, 0, false);
  writeNumberToDisplay(2, currentState->length, true, modes[0] == LEFT);
  writeNumberToDisplay(2, currentState->opticalLength, false, modes[0] == RIGHT);
  writeNumberToDisplay(3, currentState->height, true, modes[1] == LEFT);
  writeNumberToDisplay(3, currentState->opticalHeight, false, modes[1] == RIGHT);
  writeNumberToDisplay(4, save, true, modes[2] == LEFT);
  writeNumberToDisplay(4, currentState->time, false, modes[2] == RIGHT);
}

void setup() {
  for (int i = 0; i < DISPLAY_COUNT; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 0);
    lc.clearDisplay(i);
  }

  PCICR = 0b111;
  PCMSK0 = (1 << PCINT1);
  PCMSK1 = (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT11) | (1 << PCINT13);
  PCMSK2 = (1 << PCINT16) | (1 << PCINT20) | (1 << PCINT22) | (1 << PCINT23);

  // Serial.begin(115200);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);

  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);

  // pinMode(2, INPUT_PULLUP);
  // pinMode(3, INPUT_PULLUP);

  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);

  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);

  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);

  checkPins();
  initScreen();
}

void loop() {
  for (int i = 0; i < DISPLAY_COUNT; i++) {
    lc.setIntensity(i, brightness);
  }
  writeNumberToDisplay(2, currentState->length, true, modes[0] == LEFT);
  writeNumberToDisplay(2, currentState->opticalLength, false, modes[0] == RIGHT);
  writeNumberToDisplay(3, currentState->height, true, modes[1] == LEFT);
  writeNumberToDisplay(3, currentState->opticalHeight, false, modes[1] == RIGHT);
  writeNumberToDisplay(4, save, true, modes[2] == LEFT);
  writeNumberToDisplay(4, currentState->time, false, modes[2] == RIGHT);

  float distance = calculateDistance(currentState->height, currentState->opticalHeight, magnified);
  float angle = calculateAngle(distance, currentState->length, currentState->opticalLength, magnified);
  float speed = calculateSpeed(currentState->length, currentState->time);

  writeFloatToDisplay(0, distance / 100, true);
  writeFloatToDisplay(0, angle, false);
  writeFloatToDisplay(1, speed, false);

  delay(100);
}
