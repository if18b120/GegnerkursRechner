#include "LedControl.h"

#define DISPLAY_COUNT 5
#define SAVECOUNT 10

//clk white 14
//cs green 15
//din yellow 16

LedControl lc = LedControl(16, 14, 15, DISPLAY_COUNT);

enum PIN
{
    D0,
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7,
    D8,
    D9,
    D10,
    D11,
    D12
};

struct State 
{
  int length = 100;
  int height = 20;
  int opticalLength = 0;
  int opticalHeight = 0;
  int time = 0;
};

State saves[SAVECOUNT];
volatile State* currentState = &(saves[0]);

volatile int save = 0;

volatile unsigned long debounce = 0;

//PINB -> (D8-D13)
ISR(PCINT0_vect)
{
  if(!isDebounceOver()) return;
  setValueFromRotary(D8, D9, &(currentState->opticalHeight));
  setValueFromRotary(D10, D11, &(currentState->opticalLength));
}

//PINC (A0-A5) unused
ISR(PCINT1_vect) {}

//PIND (D0-D7)
ISR(PCINT2_vect) 
{
  if(!isDebounceOver()) return;
  setValueFromRotary(D0, D1, &save);
  currentState = &(saves[save]);
  setValueFromRotary(D2, D3, &(currentState->height));
  setValueFromRotary(D4, D5, &(currentState->length));
  setValueFromRotary(D6, D7, &(currentState->time));
}

bool isDebounceOver() 
{
  unsigned long now = millis();

  if (now - debounce < 40) 
  {
    return false;
  }
  debounce = now;

  return true;
}

void setup() 
{
  delay(2000);
  for (int i = 0; i < DISPLAY_COUNT; i++) 
  {
    lc.shutdown(i, false);
    lc.setIntensity(i, 0);
    lc.clearDisplay(i);
  }

  PCICR = 0b111;
  PCMSK0 = (1 << PCINT1);
  PCMSK1 = (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT11) | (1 << PCINT13);
  PCMSK2 = (1 << PCINT16) | (1 << PCINT20) | (1 << PCINT22) | (1 << PCINT23);

  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
}

void loop() 
{
  writeNumberToDisplay(2, currentState->length, true);
  writeNumberToDisplay(2, currentState->opticalLength, false);
  writeNumberToDisplay(1, currentState->height, true);
  writeNumberToDisplay(1, currentState->opticalHeight, false);
  writeNumberToDisplay(0, save, true);
  writeNumberToDisplay(0, currentState->time, false);

  float distance = calculateDistance(currentState->height, currentState->opticalHeight);
  float angle = calculateAngle(distance, currentState->length, currentState->opticalLength);
  float speed = calculateSpeed(currentState->length, currentState->time);

  writeFloatToDisplay(4, distance / 100, true);
  writeFloatToDisplay(4, angle, false);
  writeFloatToDisplay(3, speed, false);

  delay(100);
}
