/*
 * Project Chicken Door
 * Author:    Hans V. Rasmussen
 * Created:   30/03-2017 18:42
 * Modified:  13/06-2017 22:35
 * Version:   0.81
*/

/*
Version: 0.81

	Added:
	- class 'DS3231RTC_Alarms' and object thereof 'RTC_alarm'
	- 'DS3231RTC_Alarms' contains:
		~ init_alarms
		~ alarm_Check

	Changed:
	- Moved the alarm initialization, interrupt and check to Supp_Func.h
	
	Removed:
	
	
	Notes:
	- To Do: 
		~ Research and document how to change alarms and time.
		~ Make a function that can change the time/set the time by using a time type.



Version: 0.8

	Included:
	- Added the Streaming library.
	- Used https://github.com/JChristensen/DS3232RTC/issues/5#issuecomment-68143652 to find solution for more reliable alarm.
	- Arduino Core initialization.
	
	Changed:
	- Now using Arduino DS3232RTC Library v1.0
	- Got the interrupt to work and fixed the serial problem by doing the interrupt the Arduino way.
	
	Removed:
	- Got rid of the sleeping functionality for now.
	
	NOTES:
	- Major revision, remade the Arduino sketch, and using a whole different set of libraries now.

Materials List:
	- Alarm interrupt and square wave: https://github.com/JChristensen/DS3232RTC/issues/5#issuecomment-68143652
	
*/


/*Beginning of Auto generated code by Atmel studio */
#include <Arduino.h>
/*End of auto generated code by Atmel studio */

#include <DS3232RTC.h>        //http://github.com/JChristensen/DS3232RTC
#include <Streaming.h>        //http://arduiniana.org/libraries/streaming/
#include <TimeLib.h>          //http://playground.arduino.cc/Code/Time
#include <Wire.h>             //http://arduino.cc/en/Reference/Wire
/* Note about includes:
 *  DS3232RTC are the function to communicate with the timer module.
 *  Streaming is for having a more C++/CLR like way of outputting serial or LCD data.
 *  TimeLib are support functions for DS3232RTC, it includes date formats and functions.
 *  Wire is standard library included with Arduino, and is for I2C communication
 */


// Extra includes and defines:
#include "Supp_Func.h"


int main(void)
{
	init();						// Initializes the Arduino Core.
	Serial.begin(9600);			// Start the serial commuication at 9600 a baud rate.
	RTC_alarm.init_alarms();	// Start the alarms.
	
	// Local Variables:
	uint8_t alarm_stat = 0;
	
	
	// print he current time:
	Serial << "Current time is: ";
	printDateTime(RTC.get());
	Serial << endl;
	
  
	while (1)
	{
		RTC_alarm.alarm_Check(&alarm_stat);	// get the alarm status.
		
		switch(alarm_stat)					// switch statement to decide what should happen if alarm has happened.
		{
			case 1:							// alarm1:
				printDateTime( RTC.get() );
				Serial << " --> Alarm 1!" << endl;
			break;
			
			case 2:							// alarm1:
				printDateTime( RTC.get() );
				Serial << " --> Alarm 2!" << endl;
			break;
				
			default:						// if there was no alarm:
				
			break;
		}
		delay(100);	// small delay
	}
}
