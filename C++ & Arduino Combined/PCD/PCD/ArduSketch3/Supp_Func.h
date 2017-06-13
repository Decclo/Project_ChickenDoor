#ifndef Supp_Func.h
#define Supp_Func.h 1
/*
 * Supp_Func.h
 * Author:		Hans V. Rasmussen
 * Created:		13/06-2017 18:47
 * Modified:	13/06-2017 22:24
 * Version:		1.0
 * 
 * Description:
 *	This library includes some extra functionality for the DS3231.
*/

#include <DS3232RTC.h>
#include <TimeLib.h>
#include <Streaming.h>

volatile boolean alarmIsrWasCalled = false;	// Variable to check if the interrupt has happened.

// Functions:

/**
 * \brief Takes in variable of type time_t and prints contents to serial or LCD.
 * 
 * \param t
 * 
 * \return void
 */
void printDateTime(time_t t)
{
	Serial << ((day(t)<10) ? "0" : "") << _DEC(day(t)) << ' ';
	Serial << monthShortStr(month(t)) << " " << _DEC(year(t)) << ' ';
	Serial << ((hour(t)<10) ? "0" : "") << _DEC(hour(t)) << ':';
	Serial << ((minute(t)<10) ? "0" : "") << _DEC(minute(t)) << ':';
	Serial << ((second(t)<10) ? "0" : "") << _DEC(second(t));
}


/**
 * \brief Function to call when the interrupt happens, should not be used by other than the interrupt.
 * 
 * \param 
 * 
 * \return void
 */
void alarmIsr()	// INT0 triggered function.
{
	alarmIsrWasCalled = true;
}


class DS3231RTC_Alarms
{
public:
	DS3231RTC_Alarms() {}	// Constructor
	
	/**
	* \brief Initializes the alarms and the interrupt.
	* 
	* \param 
	* 
	* \return void
	*/
	void init_alarms(void)
	{
		// Setup the SQW interrupt:
		DDRD &= ~(1 << DDD2); // make INT0 an input.
		PORTD |= (1 << DDD2); // enable pull-up on INT0.
		attachInterrupt(INT0, alarmIsr, FALLING);	// Initializing the INT0 interrupt in the Arduino way.
		
		//Disable the default square wave of the SQW pin.
		RTC.squareWave(SQWAVE_NONE);
		
		//Prepare alarm1 for the interrupt.
		RTC.alarm(ALARM_1);                   //ensure RTC interrupt flag is cleared
		RTC.alarmInterrupt(ALARM_1, true);

		//Prepare alarm1 for the interrupt.
		RTC.alarm(ALARM_2);                   //ensure RTC interrupt flag is cleared
		RTC.alarmInterrupt(ALARM_2, true);
	}
	
	/**
	 * \brief Takes in pointer, checks whether alarm1 or alarm2 have been triggered.
	 * 
	 * \param uint8_t *
	 * 
	 * \return void
	 */
	void alarm_Check(uint8_t *stat)
	{
		if (alarmIsrWasCalled)	// if the interrupt has happened
		{
			if (RTC.alarm(ALARM_1))	// check if alarm1 has happened and reset it.
			{
				*stat = 1;
			}
			else if (RTC.alarm(ALARM_2))	// or else check if alarm2 has happened and reset it.
			{
				*stat = 2;
			}
			alarmIsrWasCalled = false;
		}
		else	// else return 0
		{
			
			*stat = 0;
		}
	}
	
protected:
private:
};

extern DS3231RTC_Alarms RTC_alarm;	// Make a object of the 'class DS3231RTC_Alarms' named 'RTC_alarm'

#endif