void writeFloatToDisplay(int display, float value, bool leftBound) 
{
  if (value > 99.9) value = 99.9;
  if (value < 0) value = 0;
  int num = value * 10;

  int offset = fillBlanks(display, leftBound);

  do 
  {
    int digit = num % 10;
    num /= 10;
    lc.setDigit(display, offset + index, digit, index == 1);
    index++;
  } while (num > 0);
}

void writeNumberToDisplay(int display, int value, bool leftBound) 
{
  if (value > 999) value = 999;
  if (value < 0) value = 0;

  int offset = fillBlanks(display, leftBound);
  
  do 
  {
    int digit = value % 10;
    value /= 10;
    lc.setDigit(display, offset + index, digit);
    index++;
  } while (value > 0);
}

int fillBlanks(int display, bool leftBound)
{
  int index = 0;
  int offset = 0;
  if (leftBound) 
  {
    offset = 5;
  }

  for (int i = 0; i < 3; i++) 
  {
    lc.setChar(display, offset + i, ' ', false);
  }
  return offset;
}