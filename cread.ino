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
}

bool setValueFromRotary(PIN pinA, PIN pinB, int* value)
{
    int rotDir = readRotary(pinA, pinB);
    if(rotDir == -1) (*value)--;
    else if(rotDir == 1) (*value)++;
}

int readRotary(PIN pinA, PIN pinB)
{
    int regA;
    int pinNumA;
    getPin(pinA, &regA, &pinNumA);

    int regB;
    int pinNumB;
    getPin(pinB, &regB, &pinNumB);

    if (bitRead(regA, pinNumA) == 0) 
    {
        if (bitRead(regB, pinNumB) == 0) 
        {
            return 1;
        } 
        else 
        {
            return -1;
        }
    }
    return 0;
}

void getPin(PIN pin, int* reg, int* pinNum)
{
    switch(pin)
    {
        case D0:
            *reg = PIND;
            *pinNum = 0;
            break;
        case D1:
            *reg = PIND;
            *pinNum = 1;
            break;
        case D2:
            *reg = PIND;
            *pinNum = 2;
            break;
        case D3:
            *reg = PIND;
            *pinNum = 3;
            break;
        case D4:
            *reg = PIND;
            *pinNum = 4;
            break;
        case D5:
            *reg = PIND;
            *pinNum = 5;
            break;
        case D6:
            *reg = PIND;
            *pinNum = 6;
            break;
        case D7:
            *reg = PIND;
            *pinNum = 7;
            break;
        case D8:
            *reg = PINB;
            *pinNum = 0;
            break;
        case D9:
            *reg = PINB;
            *pinNum = 1;
            break;
        case D10:
            *reg = PINB;
            *pinNum = 2;
            break;
        case D11:
            *reg = PINB;
            *pinNum = 3;
            break;
        case D12:
            *reg = PINB;
            *pinNum = 4;
            break;
    }
}