#ifndef Supp_Func.h
#define Supp_Func.h 1
/*
 * Supp_Func.h
 * Author:		Hans V. Rasmussen
 * Created:		13/06-2017 18:47
 * Modified:	14/06-2017 18:21
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

void alarmIsr()	// INT0 triggered function.
{
	alarmIsrWasCalled = true;
}


class Human_Machine_Interface
{
public:
	Human_Machine_Interface() {}
	
	/**
	* \brief Takes in variable of type time_t and prints contents to serial or LCD. 
	*	Text is 20 characters long.
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
	 * \brief Takes in variable of type tmElements_t and prints contents to serial or LCD. 
	 *	Does not print year, text is 15 characters long.
	 * 
	 * \param TM
	 * 
	 * \return void
	 */
	void printDateTime(tmElements_t TM)
	{
		Serial << ((TM.Day<10) ? "0" : "") << TM.Day << ' ';
		Serial << monthShortStr(TM.Month) << ' ';
		Serial << ((TM.Hour<10) ? "0" : "") << TM.Hour << ':';
		Serial << ((TM.Minute<10) ? "0" : "") << TM.Minute << ':';
		Serial << ((TM.Second<10) ? "0" : "") << TM.Second << endl;
	}
	
	/**
	 * \brief Takes in time_t and converts it to tmElements_t.
	 * 
	 * \param t
	 * 
	 * \return tmElements_t
	 */
	tmElements_t ConvTotm(time_t t)
	{
		tmElements_t TM;
		TM.Year = year(t);
		TM.Day = day(t);
		TM.Month = month(t);
		TM.Hour = hour(t);
		TM.Minute = minute(t);
		TM.Second = second(t);
		
		return TM;
	}
	
protected:
private:
};


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
	
	/**
	 * \brief Takes in tmElements_t and sets timer1 to trigger once a day.
	 *	Only uses seconds, minutes and hours.
	 * 
	 * \param TM
	 * 
	 * \return boolean
	 */
	void alarm1_set(tmElements_t TM)
	{
		RTC.setAlarm(ALM1_MATCH_HOURS, TM.Second, TM.Minute, TM.Hour, 1);		//daydate parameter should be between 1 and 7
		RTC.alarm(ALARM_1);														//ensure RTC interrupt flag is cleared
		RTC.alarmInterrupt(ALARM_1, true);
	}
	
	/**
	 * \brief Takes in tmElements_t and sets timer2 to trigger once a day.
	 *	Only uses seconds, minutes and hours.
	 * 
	 * \param TM
	 * 
	 * \return boolean
	 */
	void alarm2_set(tmElements_t TM)
	{
		RTC.setAlarm(ALM2_MATCH_HOURS, TM.Second, TM.Minute, TM.Hour, 1);		//daydate parameter should be between 1 and 7
		RTC.alarm(ALARM_2);														//ensure RTC interrupt flag is cleared
		RTC.alarmInterrupt(ALARM_2, true);
	}
	
	
	/************************************************************************/
	/*	To change alarms do the following:                        
	          
	RTC.setAlarm(ALM1_MATCH_SECONDS, 0, 0, 0, 1);		//daydate parameter should be between 1 and 7
	RTC.alarm(ALARM_1);									//ensure RTC interrupt flag is cleared
	RTC.alarmInterrupt(ALARM_1, true);
	
	/*	RTC.setAlarm syntax for alarm1 is 
	void setAlarm(ALARM_TYPES_t alarmType, byte seconds, byte minutes, byte hours, byte daydate);
	/*
	/*	RTC.setAlarm syntax for alarm2 is 
	void setAlarm(ALARM_TYPES_t alarmType, byte minutes, byte hours, byte daydate);
	/*
	/*
	Values for Alarm 1
	ALM1_EVERY_SECOND -- causes an alarm once per second.
	ALM1_MATCH_SECONDS -- causes an alarm when the seconds match (i.e. once per minute).
	ALM1_MATCH_MINUTES -- causes an alarm when the minutes and seconds match.
	ALM1_MATCH_HOURS -- causes an alarm when the hours and minutes and seconds match.
	ALM1_MATCH_DATE -- causes an alarm when the date of the month and hours and minutes and seconds match.
	ALM1_MATCH_DAY -- causes an alarm when the day of the week and hours and minutes and seconds match.
	
	Values for Alarm 2
	ALM2_EVERY_MINUTE -- causes an alarm once per minute.
	ALM2_MATCH_MINUTES -- causes an alarm when the minutes match (i.e. once per hour).
	ALM2_MATCH_HOURS -- causes an alarm when the hours and minutes match.
	ALM2_MATCH_DATE -- causes an alarm when the date of the month and hours and minutes match.
	ALM2_MATCH_DAY -- causes an alarm when the day of the week and hours and minutes match.
	/*
	/************************************************************************/
	
protected:
private:
};

// make objects of the classes:
DS3231RTC_Alarms RTC_alarm;	// Make a object of the 'class DS3231RTC_Alarms' named 'RTC_alarm'
Human_Machine_Interface HMI;

#endif