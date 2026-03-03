#define MSTOKTCONVERSION 1.94384

float calculateDistance(float height, float opticalHeight) 
{
    if (opticalHeight <= 0) 
    {
        return 999;
    }

    opticalHeight /= 4;  // Correct the angular size
    
    return height / tan(radians(opticalHeight / 16));
}

float calculateAngle(float distance, float length, float opticalLength) 
{
    if (length <= 0) 
    {
        return 0;
    }
    
    opticalLength /= 4;  // Correct the angular size
    
    float apparentWidth = 2 * distance * tan(radians(opticalLength) / 2);
    float AoB = degrees(acos(apparentWidth / length));
    return AoB;
}

float calculateSpeed(float length, float time) 
{
    if (time <= 0) return 999;
    return length / time * MSTOKTCONVERSION;
}