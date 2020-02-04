#include "hw_timer.h"          
const byte zcPin = 5;
const byte pwmPin = 14;  

byte tarBrightness = 0;
byte curBrightness = 0;
byte zcState = 0;   // 0 = ready; 1 = processing;
bool modeOff;       // false = on; true = off used to prevent un-necassary writes to output
bool modeOn;        // false = on; true = off used to prevent un-necassary writes to output

void ICACHE_RAM_ATTR zcDetectISR ();//required for proper interrupting

void setup() {
  Serial.begin(115200);   
  pinMode(zcPin, INPUT_PULLUP);
  pinMode(pwmPin, OUTPUT);
  attachInterrupt(zcPin, zcDetectISR, RISING);    // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
  hw_timer_init(NMI_SOURCE, 0);
  hw_timer_set_func(dimTimerISR);
}

void loop() 
{
  if (Serial.available())
  {
    int val = Serial.parseInt();
    if (val == -1)
    {
      tarBrightness = 0;
      modeOff = true;
    }
    if (val>0)
    {
      tarBrightness = val;
      Serial.println(tarBrightness);
      modeOn = true;
    }
  }
}

void dimTimerISR() 
{
  if (curBrightness > tarBrightness)
  {
    --curBrightness;
  }
  else if (curBrightness < tarBrightness)
  {
    ++curBrightness;
  }
  
  switch(curBrightness)
    {
      case 0:
        if (modeOff == true)
        {
          digitalWrite(pwmPin, 0);
          modeOff = false;
        }
      break;

      case 255:
        if (modeOn == true)
        {
          digitalWrite(pwmPin, 1);
          modeOn = false;
        }
      break;

      default:
        digitalWrite(pwmPin, 1);
      break;
    }
  zcState = 0;
}

void zcDetectISR()
{
  if (zcState == 0)
  {
    zcState = 1;
    if (curBrightness < 255 && curBrightness > 0)
    {
      digitalWrite(pwmPin, 0);
      int dimDelay = 30 * (255 - curBrightness) + 400;//400
      hw_timer_arm(dimDelay);
    }
  }
}
