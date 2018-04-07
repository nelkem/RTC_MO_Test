#include <Wire.h>
#include <RTClibExtended.h>
#include "LowPower.h"

#define RTC3231
#define AlarmType 1

#ifdef RTC3231
RTC_DS3231 myRtc;
#endif

volatile bool TakeSampleFlag = false;
volatile int SEC = 0;
volatile int HR = 0;
volatile int MIN = 0;
volatile int WakePeriodMin = 0;
volatile int WakePeriodSec = 10;
const byte INT_PIN = 10;
DateTime zeros = DateTime (0, 0, 0, 0, 0, 0);

void setup() {
  delay(5000);
  Serial.begin(9600);
  Serial.println("RTC Pint Interrupt Test");
  InitializeIO();
  InitializeRTC();
  Serial.print("Alarm set to go off every " + String(WakePeriodMin) + " min, ");
  Serial.println(String(WakePeriodSec) + " secs from starting time");
  delay(1000);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), wake, FALLING);
  Serial.println("Sleeping until interrupt...");
  Serial.println(digitalPinToInterrupt(INT_PIN));
}

void loop() {
  LowPower.idle(IDLE_1);
  //while(!TakeSampleFlag)
    

  if(TakeSampleFlag){
    clearAlarmFunction();
    Serial.begin(9600);
    delay(50);
    setAlarmFunction();
    delay(75);
    TakeSampleFlag=false;
    delay(1);
    Serial.println("Sleeping until interrupt...");
    Serial.println(IDLE_0);
    attachInterrupt(digitalPinToInterrupt(INT_PIN), wake, FALLING);
  }
}

/*
 * *****************************************************************************
 */
void InitializeIO(){
  pinMode(INT_PIN, INPUT_PULLUP);
}

void wake(){
  detachInterrupt(digitalPinToInterrupt(INT_PIN));
  TakeSampleFlag = true;
}

void InitializeRTC(){
  if(!myRtc.begin()){
    Serial.println("Couldn't find RTC");
    while(1);
  }
  clearAlarmFunction();
  myRtc.adjust(zeros);
  DateTime now = myRtc.now();
  Serial.print  ("RTC Time is: ");
  Serial.print  (now.hour(),   DEC); Serial.print(':');
  Serial.print  (now.minute(), DEC); Serial.print(':');
  Serial.println(now.second(), DEC);

  myRtc.writeSqwPinMode(DS3231_OFF);

  setAlarmFunction();
}


void setAlarmFunction(){
  if(AlarmType == 0){
    Serial.print(F("the current sample Alarm set for Daily at"));
  }
  else{
    Serial.print("Resetting Periodic Alarm For: ");
  }
  myRtc.adjust(zeros);
  myRtc.setAlarm(ALM1_MATCH_HOURS, WakePeriodSec, WakePeriodMin, 0, 0);
  myRtc.alarmInterrupt(1, true);
  Serial.print  (String(0) + ":");
  Serial.print  (String(WakePeriodMin) + ":");
  Serial.println(String(WakePeriodSec));
}

void clearAlarmFunction(){
  myRtc.armAlarm(1, false);
  myRtc.clearAlarm(1);
  myRtc.alarmInterrupt(1, false);
  myRtc.armAlarm(2, false);
  myRtc.clearAlarm(2);
  myRtc.alarmInterrupt(2, false);
}

