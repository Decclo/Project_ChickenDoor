/*
 * PCD_Initial_Setup
 * 
 * created: 13/06-2017 17:41
 * By: Hans Rasmussen
 */

#include <DS3232RTC.h>        //http://github.com/JChristensen/DS3232RTC
#include <Streaming.h>        //http://arduiniana.org/libraries/streaming/
#include <TimeLib.h>          //http://playground.arduino.cc/Code/Time
#include <Wire.h>             //http://arduino.cc/en/Reference/Wire
#include <LiquidCrystal.h>
/*
 * Note about includes:
 *  DS3232RTC are the function to communicate with the timer module.
 *  Streaming is for having a more C++/CLR like way of outputting serial or LCD data.
 *  TimeLib are support functions for DS3232RTC, it includes date fomats and functions.
 *  Wire is standard library included with arduino, and is for I2C communication.
 *  LiquidCrystal is standard library for using a LCD screen.
 */

 LiquidCrystal lcd(13, 12, 8, 9, 10, 11);  //(Pro Mini)

void setup() {
  // put your setup code here, to run once:
  init();
  lcd.begin(16, 2);     // Start LCD.
  lcd.clear();

  tmElements_t TM1; // Alarm 1 decides when to open the door.
  TM1.Second = 0;
  TM1.Minute = 0;
  TM1.Hour = 7;

  tmElements_t TM2; // Alarm2 decides when to close the door.
  TM2.Second = 0; // alarm 2: seconds must be given but are ignored. Will always trigger at 00.
  TM2.Minute = 0;
  TM2.Hour = 21;

  // SetAlarm(AlarmType, Seconds, Minutes, Hours, 1);

  // Set alarm to interrupt ever day
  RTC.setAlarm(ALM1_MATCH_HOURS, TM1.Second, TM1.Minute, TM1.Hour, 1);
  RTC.setAlarm(ALM2_MATCH_HOURS, TM2.Second, TM2.Minute, TM2.Hour, 1);

  // Set alarm to interrupt every hour
  //RTC.setAlarm(ALM1_MATCH_MINUTES, TM1.Second, TM1.Minute, TM1.Hour, 1);
  //RTC.setAlarm(ALM2_MATCH_MINUTES, TM2.Second, TM2.Minute, TM2.Hour, 1);

  // Set alarm to interrupt every minute
  //RTC.setAlarm(ALM1_MATCH_SECONDS, TM1.Second, TM1.Minute, TM1.Hour, 1);
  //RTC.setAlarm(ALM2_EVERY_MINUTE, TM2.Second, TM2.Minute, TM2.Hour, 1);

  lcd << "Flash Complete";

}

void loop() {
  // put your main code here, to run repeatedly:

}

