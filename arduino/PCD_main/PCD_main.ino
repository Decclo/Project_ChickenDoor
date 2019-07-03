/*
 * Project Chicken Door
 * Author:    Hans Vanselow-Rasmussen
 * Created:   03/07-2019 13:36
 * Modified:  03/07-2019 13:36
 * Version:   0.1
 * Description:
 *  Code to open and close a door by controlling a relay H-bridge for a AC lift.
 *  Uses a DS3231 for timekeeping, and has LCD for runtime adjustments.
*/


/*** Include libraries and define defines. ***/
// Standard libraries
#include <Streaming.h>      // Library that makes it possible to use the 'cout <<' syntax.
#include <TimeLib.h>        // Library for time structures.
#include <Time.h>
#include <Wire.h>           // Library for I2C communication.
#include <DS3232RTC.h>      // Library for DS3231 interfacing, requires 'Wire.h'
#include <LiquidCrystal.h>  // Library for using the LCD display.

// Support functions
#include "Supp_Func.h"

LiquidCrystal lcd(13, 12, 8, 9, 10, 11);  // Start the LCD display (Nano(Pro Mini))

/*** Setup ***/
void setup() {
  // Initialize classes and communication protocols
  
  Serial.begin(9600);                       // Start the serial communication at 9600 baud
  relayArray.relayArrayInit();              // Start the relays
  bool debug = false;                       // Wether debugging is in progress.
  bool settime_on = true;                   // Wether readjusting the time is in progress

  lcd.begin(16, 2);     // Start LCD.
  RTC_alarm.init_alarms();  // Start the alarms.

  // Variables for setting the time.
  static time_t tLast;
  time_t t;
  tmElements_t tm;
  
  // Give debug info over serial:
  Serial << "Project Chicken Door - version 0.90" << endl << endl;
  Serial << "Please press Enter to engage debugging mode." << endl;
  Serial << "PCD going online in" << endl;
  Serial << "3..." << endl;
  delay(1000);
  Serial << "2..." << endl;
  delay(1000);
  Serial << "1..." << endl;
  delay(1000);

  // check wether to enter debug mode:
  if(Serial.available())
  {
    debug = true;
    Serial << "\n\n\nDebugging engaged at ";
    HMI.printDateTime(RTC.get());
    Serial << endl << "Alarm 1 is set to open at ";
    HMI.printDateTime(RTC_alarm.alarm1_get());
    Serial << endl << "Alarm 2 is set to close at ";
    HMI.printDateTime(RTC_alarm.alarm2_get());

    char serialInput = NULL;      // a String to hold incoming data

    while(1)
    {
      if(serialInput)
      {
        Serial << "Please Choose from one of the following categories:" << endl;
        Serial << "    1. Lift Extend" << endl;
        Serial << "    2. Lift Retract" << endl;
        Serial << "    3. Lift Stop" << endl;
        Serial << "    4. Set Current Time" << endl;
        Serial << "    5. Set Alarm 1 (open door)" << endl;
        Serial << "    6. Set Alarm 2 (close door)" << endl;
        Serial << "    0. Continue running the program" << endl;
      }

      while(!Serial.available())
      {
        delay(10);
      }
      serialInput = Serial.read();
      switch (serialInput)
      {
        case 49: // 1
          Serial << "\nLift Extending!\n" << endl;
          relayArray.relayArrayCommand(liftCCW);
          break;

        case 50: // 2
          Serial << "\nLift Retracting!\n" << endl;
          relayArray.relayArrayCommand(liftCW);
          break;

        case 51: // 3
          Serial << "\nLift Stopping!\n" << endl;
          relayArray.relayArrayCommand(liftSTOP);
          break;
        case 52: // 4
          Serial << "\nPlease enter the current data in the format yy,mm,dd,hh,mm,ss\n" << endl;
          settime_on = true;
          while(settime_on == 1)
          {
            if(Serial.available() >= 12)
            {
              int y = Serial.parseInt();
              if(y >= 100 && y < 1000)
              {
                Serial << F("Error: Year must be two digits or four digits!") << endl;
              }
              else
              {
                if (y >= 1000)
                    tm.Year = CalendarYrToTm(y);
                else    // (y < 100)
                    tm.Year = y2kYearToTm(y);
                tm.Month = Serial.parseInt();
                tm.Day = Serial.parseInt();
                tm.Hour = Serial.parseInt();
                tm.Minute = Serial.parseInt();
                tm.Second = Serial.parseInt();
                t = makeTime(tm);
                RTC.set(t);        // use the time_t value to ensure correct weekday is set
                setTime(t);
                Serial << F("RTC set to: ");
                printDateTime(t);
                Serial << endl << endl;
                // dump any extraneous input
                while (Serial.available() > 0) Serial.read();
                settime_on = false;
              }
            }
          }
          break;

        case 53: // 5
          Serial << "\nPlease enter when to open the door in  hh,mm\n" << endl;
          settime_on = true;
          while(settime_on == 1)
          {
            if(Serial.available() >= 5)
            {
              int h = Serial.parseInt();
              if(h < 0 && h > 23)
              {
                Serial << F("Error: Hour must be between 0 and 23!") << endl;
              }
              else
              {
                t = RTC.get();
                tm.Year = CalendarYrToTm(year(t));
                tm.Month = month(t);
                tm.Day = day(t);
                tm.Hour = Serial.parseInt();
                tm.Minute = Serial.parseInt();
                tm.Second = 0;
                RTC_alarm.alarm1_set(tm);
                Serial << "RTC set to: " << ((tm.Hour<10) ? "0" : "") << tm.Hour << ":" << ((tm.Minute<10) ? "0" : "") << tm.Minute << "";
                t = RTC_alarm.alarm1_get();
                Serial << endl << endl;
                // dump any extraneous input
                while (Serial.available() > 0) Serial.read();
                settime_on = false;
              }
            }
          }
          break;

        case 54: // 6
          Serial << "\nPlease enter when to open the door in  hh,mm\n" << endl;
          settime_on = true;
          while(settime_on == 1)
          {
            if(Serial.available() >= 5)
            {
              int h = Serial.parseInt();
              if(h < 0 && h > 23)
              {
                Serial << F("Error: Hour must be between 0 and 23!") << endl;
              }
              else
              {
                t = RTC.get();
                tm.Year = CalendarYrToTm(year(t));
                tm.Month = month(t);
                tm.Day = day(t);
                tm.Hour = Serial.parseInt();
                tm.Minute = Serial.parseInt();
                tm.Second = 0;
                RTC_alarm.alarm2_set(tm);
                Serial << "RTC set to: " << ((tm.Hour<10) ? "0" : "") << tm.Hour << ":" << ((tm.Minute<10) ? "0" : "") << tm.Minute << "";
                t = RTC_alarm.alarm2_get();
                Serial << endl << endl;
                // dump any extraneous input
                while (Serial.available() > 0) Serial.read();
                settime_on = false;
              }
            }
          }
          break;

        case 48: // 0
          Serial << "\nDebugger Exiting\nResuming normal operation...\n" << endl;
          debug = false;
          break;
        
        case 0: // NULL
          break;
        
        default:
          Serial << "Command not recognized, try again:" << endl;
          break;
      }

      if(!debug)
      {
        relayArray.relayArrayCommand(liftSTOP);
        break;
      }
    }
  }
  
  // Print the current time:
  Serial << "PCD going online at: ";
  HMI.printDateTime(RTC.get());
  Serial << endl;
  
}


/*** Main function ***/
void loop() {
  // Local Variables:
  uint8_t alarm_stat = 0;
  
  // get the alarm status.
  RTC_alarm.alarm_Check(&alarm_stat); 
  
  // switch statement to decide what should happen if alarm has happened.
  // This step is not really required, as the relayArray.relayAutoCommand() takes in the value of RTC_alarm.alarm_Check.
  switch(alarm_stat)
  {
    case 1: // alarm1:
        // Print on serial that alarm has triggered.
      HMI.printDateTime( RTC.get() );
      Serial << " --> Alarm 1 triggered!" << endl;
      
        // Make motor turn CW (Open Door)
      relayArray.relayAutoCommand(1); 
    break;
    
    case 2: // alarm2:
        // Print on serial that alarm has triggered.
      HMI.printDateTime( RTC.get() );
      Serial << " --> Alarm 2 triggered!" << endl;
      
        // Make motor turn CCW (Close Door)
      relayArray.relayAutoCommand(2);
    break;
      
    default:            // if there was no alarm:
      relayArray.relayAutoCommand(0);
    break;
  }
  
  // run standard tasks:
  HMI.UIupdate();
  
  delay(100); // small delay

}


/*** Additional functions ***/
// print date and time to Serial
void printDateTime(time_t t)
{
    printDate(t);
    Serial << ' ';
    printTime(t);
}

// print time to Serial
void printTime(time_t t)
{
    printI00(hour(t), ':');
    printI00(minute(t), ':');
    printI00(second(t), ' ');
}

// print date to Serial
void printDate(time_t t)
{
    printI00(day(t), 0);
    Serial << monthShortStr(month(t)) << _DEC(year(t));
}

// Print an integer in "00" format (with leading zero),
// followed by a delimiter character to Serial.
// Input value assumed to be between 0 and 99.
void printI00(int val, char delim)
{
    if (val < 10) Serial << '0';
    Serial << _DEC(val);
    if (delim > 0) Serial << delim;
    return;
}
