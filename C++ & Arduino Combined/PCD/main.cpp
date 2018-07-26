/*
 * Project Chicken Door
 * Author:    Hans V. Rasmussen
 * Created:   30/03-2017 18:42
 * Modified:  30/07-2017 16:32
 * Version:   0.84
*/

//Change log
/*
Version: 0.90

Added:
	- Debugging tools

Changed:
	- Moved away from the homebrew Atmel Studio Environment.

Removed:


Notes:
	- PCD can currently be used with either the Arduino IDE or with a makefile. Makefile is included.



Version: 0.84

Added:
	- Made the signal for the motor turn off after 10 seconds, to be changed in the future.
	- Made the program compatible with the Arduino Uno by changing the LCD initialization (must be done manually).
	- More advanced debugging

Changed:
	- Minor changes to relay
	- Changed the time to turn off the motor to 4 seconds.

Removed:


Notes:
	- This version underwent tests to make sure it will work with the electronics. 
	- Right now the alarms have to be set using a separate program, to be changed before version 1.0
	- Mechanics and hardware finished, this is the first version actually running the door.


Version: 0.83

	Added:
	- added EEPROM, relay and UI functionality in Supp_Func.h
	- Adding LCD functionality, including libraries.

	Changed:

	
	Removed:
	
	
	Notes:


Version: 0.82

	Added:
	- Made research and comments about how to set the timers. 
	- Made new class in Supp_Func.h called 'Human_Machine_Interface', this class and its object 'HMI' contain functions to interact with the user.
	- Added functions to use tmElements_t to set timers.
	
	Changed:
	
	
	Removed:
	
	
	Notes:
		- Next version will be about introducing the EEPROM to store the alarm values for later.


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
	- Got the interrupt to work and fixed the serial problem by doing the interrupt the Arduino way, serial write still not working.
	
	Removed:
	- Got rid of the sleeping functionality for now.
	
	NOTES:
	- Major revision, remade the Arduino sketch, and using a whole different set of libraries now.


Materials List:
	- Alarm interrupt and square wave: https://github.com/JChristensen/DS3232RTC/issues/5#issuecomment-68143652
	
*/


#include <DS3232RTC.h>				//http://github.com/JChristensen/DS3232RTC
#include <Streaming.h>				//http://arduiniana.org/libraries/streaming/
#include <TimeLib.h>					//http://playground.arduino.cc/Code/Time
#include <Wire.h>						//http://arduino.cc/en/Reference/Wire
#include <LiquidCrystal.h>			// Arduino library for LCD
/* Note about includes:
 *  DS3232RTC are the function to communicate with the timer module.
 *  Streaming is for having a more C++/CLR like way of outputting serial or LCD data.
 *  TimeLib are support functions for DS3232RTC, it includes date formats and functions.
 *  Wire is standard library included with Arduino, and is for I2C communication.
 *  LiquidCrystal is standard library included with Arduino for LCD communication.
 */


// Extra includes and defines:
#include "./src/Supp_Func.h"			// Self made library containing classes and functions for the main function.


/* Initialize the LiquidCrystal library with the numbers of the interface pins
  The circuit:
  * LCD RS pin to digital pin 13
  * LCD Enable pin to digital pin 12
  * LCD D4 pin to digital pin 8
  * LCD D5 pin to digital pin 9
  * LCD D6 pin to digital pin 10
  * LCD D7 pin to digital pin 11
  * LCD R/W pin to ground
  * LCD VSS pin to ground
  * LCD VCC pin to 5V
  * 10K resistor: Mounted
  * ends to +5V and ground
  */
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);	//(UNO)
LiquidCrystal lcd(13, 12, 8, 9, 10, 11);	//(Nano(Pro Mini))


int main(void)
{
	init();						// Initializes the Arduino Core.
	Serial.begin(9600);			// Start the serial communication at 9600 baud rate.
	relayArray.relayArrayInit();// Start the relays.
	bool debug = false;			// Wether debugging is in progress.

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
		Serial << endl;

		char serialInput = NULL;			// a String to hold incoming data

		while(1)
		{
			if(serialInput)
			{
				Serial << "Please Choose from one of the following categories:" << endl;
				Serial << "    1. Lift Extend" << endl;
				Serial << "    2. Lift Retract" << endl;
				Serial << "    3. Lift Stop" << endl;
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


	lcd.begin(16, 2);			// Start LCD.
	RTC_alarm.init_alarms();	// Start the alarms.
	

	// Local Variables:
	uint8_t alarm_stat = 0;
	
	// Print the current time:
	Serial << "PCD going online at: ";
	HMI.printDateTime(RTC.get());
	Serial << endl;

	while (1)
	{
		// get the alarm status.
		RTC_alarm.alarm_Check(&alarm_stat);	
		// switch statement to decide what should happen if alarm has happened.
		// This step is not really required, as the relayArray.relayAutoCommand() takes in the value of RTC_alarm.alarm_Check.
		switch(alarm_stat)					
		{
			case 1:	// alarm1:
					// Print on serial that alarm has triggered.
				HMI.printDateTime( RTC.get() );
				Serial << " --> Alarm 1 triggered!" << endl;
				
					// Make motor turn CW (Open Door)
				relayArray.relayAutoCommand(1);	
			break;
			
			case 2:	// alarm2:
					// Print on serial that alarm has triggered.
				HMI.printDateTime( RTC.get() );
				Serial << " --> Alarm 2 triggered!" << endl;
				
					// Make motor turn CCW (Close Door)
				relayArray.relayAutoCommand(2);
			break;
				
			default:						// if there was no alarm:
				relayArray.relayAutoCommand(0);
			break;
		}
		
		// run standard tasks:
		HMI.UIupdate();
		
		delay(1000);	// small delay
	}
}
