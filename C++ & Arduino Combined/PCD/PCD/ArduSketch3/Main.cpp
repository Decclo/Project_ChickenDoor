/*
 * Project Chicken Door
 * Author:    Hans V. Rasmussen
 * Created:   30/03-2017 18:42
 * Modified:  13/06-2017 20:40
 * Version:   0.81
*/

/*
Version: 0.81

	Added:
	
	Changed:
	
	Removed:
	
	Notes:


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


// Global Variables:
volatile boolean alarmIsrWasCalled = false;	// Variable to check if the interrupt has happened.


// Function Prototypes:
void alarmIsr();	// Interrupt function prototype, Arduino style!


int main(void)
{
	init();	// Initializes the Arduino Core.
	
	// Starting serial at 9600 a baud rate.
	Serial.begin(9600);
	
	// print he current time:
	Serial << "Current time is: ";
	printDateTime(RTC.get());
	Serial << endl;
	
	
	// Setup the SQW interrupt:
	DDRD &= ~(1 << DDD2); // make INT0 an input.
	PORTD |= (1 << DDD2); // enable pull-up on INT0.
	attachInterrupt(INT0, alarmIsr, FALLING);	// Initializing the INT0 interrupt in the Arduino way.
	
	//Disable the default square wave of the SQW pin.
	RTC.squareWave(SQWAVE_NONE);
	
	//Set an alarm at every 20th second of every minute.
	RTC.setAlarm(ALM1_MATCH_SECONDS, 20, 0, 0, 1);    //daydate parameter should be between 1 and 7
	RTC.alarm(ALARM_1);                   //ensure RTC interrupt flag is cleared
	RTC.alarmInterrupt(ALARM_1, true);

	//Set an alarm every minute.
	RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0, 1);    //daydate parameter should be between 1 and 7
	RTC.alarm(ALARM_2);                   //ensure RTC interrupt flag is cleared
	RTC.alarmInterrupt(ALARM_2, true);
  
  
	while (1)
	{
		if (alarmIsrWasCalled)
		{
			if (RTC.alarm(ALARM_1))
			{
				printDateTime( RTC.get() );
				Serial << " --> Alarm 1!" << endl;
			}
			if (RTC.alarm(ALARM_2))
			{
				printDateTime( RTC.get() );
				Serial << " --> Alarm 2!" << endl;
			}
			alarmIsrWasCalled = false;
		}
	}
}

void alarmIsr()	// INT0 triggered function.
{
	alarmIsrWasCalled = true;
}
