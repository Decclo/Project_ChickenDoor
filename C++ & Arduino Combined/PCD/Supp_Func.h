#ifndef Supp_Func.h
#define Supp_Func.h 1
/*
 * Supp_Func.h
 * Author:		Hans V. Rasmussen
 * Created:		13/06-2017 18:47
 * Modified:	30/06-2017 11:43
 * Version:		1.1
 * 
 * Description:
 *	This library includes some extra functionality for the DS3231.
 */

//Change log
/*
Version: 1.1

	Added:
	- UIupdate in Human_Machine_Interface
	- EEPROM functionality
		- Alarm1 and 2 are stored in EEPROM simultaneously with being written to the DS3231
	- LCD functionality
	- UI functionality (Work In Progress)
	- Added timer1 to make interrupt ever 1 ms
		- Added counter for relayArray to make relays turn off after x seconds
		- Added timer for UI to make a 300ms delay as a safety against the user
		- Added the button read function to the timer, so that the UI is more smooth. Hopefully this will not make any complications.
	- relayArray function to control the relays
	
	Changed:
	
	
	Removed:
	
	
	Notes:


Version: 1.0

	Added:
	- classes 'Human_Machine_Interface' and 'DS3231RTC_Alarms'

	Changed:
	
	
	Removed:
	
	
	Notes:
		- First working version
*/

#include <DS3232RTC.h>
#include <TimeLib.h>
#include <Streaming.h>
#include <LiquidCrystal.h>

// Define Buttons for LCD
#define btnPIN		A0
#define btnRESET	0
#define btnSELECT	1
#define btnRIGHT	2
#define btnUP		3
#define btnDOWN		4
#define btnLEFT		5

// Define time to wait between buttons presses (change this time to finetune the button handling)
#define UIbtnHold	500

// Define commands for Relay Array
#define liftSTOP	0
#define liftCW		1
#define liftCCW		2

// define pins of Relay Array
#define RAControl1	DDD4
#define RAControl2	DDD5
#define RAControl3	DDD6
#define RAControl4	DDD7

// Define time for Relay Array to stop lift again (ms)
#define RAHold		10000

// Declare external global lcd
extern LiquidCrystal lcd;

// Global variables:

volatile boolean	alarmIsrWasCalled = false;	// Variable to check if the interrupt has happened.

// UI:
volatile uint8_t	btnStat = 0;	// variable for testing button state every 1 ms.

volatile uint16_t	UIdelay = 0;
volatile boolean	UIdelayStat = 0;

// relayArray:
volatile uint16_t	RACounter1 = 0;
volatile boolean	RACounter1Status = 0;

// Debugging:
volatile uint16_t	T1Timer = 0;
volatile uint8_t	test = 0;

// addresses of the alarms on the EEPROM (one time_t object takes 7 bytes)
uint8_t		alarm1_addr = 0;
uint8_t		alarm2_addr = 7;


// Functions:

void alarmIsr()	// INT0 triggered function.
{
	alarmIsrWasCalled = true;
}


// Classes

class Human_Machine_Interface
{
public:
	Human_Machine_Interface();
	
	/**
	* \brief Takes in variable of type time_t and prints contents to serial or LCD. 
	*	Text is 20 characters long.
	* 
	* \param t
	* 
	* \return void
	 */
	void printDateTime(time_t t);
	
	/**
	 * \brief Takes in variable of type tmElements_t and prints contents to serial or LCD. 
	 *	Does not print year, text is 15 characters long.
	 * 
	 * \param TM
	 * 
	 * \return void
	 */
	void printDateTime(tmElements_t TM);
	
	/**
	 * \brief Takes in time_t and converts it to tmElements_t.
	 * 
	 * \param t
	 * 
	 * \return tmElements_t
	 */
	tmElements_t ConvTotm(time_t t);

	/**
	 * \brief Checks LCD buttons and returns value corresponding to button.
	 * 
	 * \param void
	 * 
	 * \return uint8_t
	 */
	uint8_t read_LCD_buttons(void);
	
	/**
	 * \brief handles user input/output, should be called regurlarly
	 * 
	 * \param void
	 * 
	 * \return void
	 */
	void UIupdate(void);

protected:
private:
	uint8_t UIstate;
	tmElements_t tid;
};


class DS3231RTC_Alarms
{
public:
	DS3231RTC_Alarms();	// Constructor
	
	/**
	* \brief Initializes the alarms and the interrupt.
	* 
	* \param 
	* 
	* \return void
	*/
	void init_alarms(void);
	
	/**
	 * \brief Takes in pointer, checks whether alarm1 or alarm2 have been triggered.
	 * 
	 * \param uint8_t *
	 * 
	 * \return void
	 */
	void alarm_Check(uint8_t *stat);
	
	/**
	 * \brief returns the time when alarm1 is expected
	 * 
	 * \param void
	 * 
	 * \return time_t
	 */
	time_t alarm1_get(void);

	/**
	 * \brief returns the time when alarm2 is expected
	 * 
	 * \param void
	 * 
	 * \return time_t
	 */
	time_t alarm2_get(void);

	/**
	 * \brief Takes in tmElements_t and sets timer1 to trigger once a day.
	 *	Only uses seconds, minutes and hours.
	 * 
	 * \param TM
	 * 
	 * \return boolean
	 */
	void alarm1_set(tmElements_t TM);
	
	/**
	 * \brief Takes in tmElements_t and sets timer2 to trigger once a day.
	 *	Only uses seconds, minutes and hours.
	 * 
	 * \param TM
	 * 
	 * \return boolean
	 */
	void alarm2_set(tmElements_t TM);
	
	
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
	union {
		unsigned long int long_time;
		 byte byte_array[7];
	} alarm1_time;

	union {
		unsigned long int long_time;
		byte byte_array[7];
	} alarm2_time;
};


class liftRelayArray
{
public:
	liftRelayArray();	// Constructor

	void relayArrayInit(void);

	/**
	 * \brief Executes a command for the lift
	 * 
	 * \param cmd - liftCW, liftCCW, liftSTOP
	 * 
	 * \return void
	 */
	void relayArrayCommand(uint8_t cmd);
	
	void relayAutoCommand(boolean alarm1trig, boolean alarm2trig);
	
protected:
private:
};


// make objects of the classes:
Human_Machine_Interface HMI;// Make a object of the 'class Human_Machine_Interface' named 'HMI'
DS3231RTC_Alarms RTC_alarm;	// Make a object of the 'class DS3231RTC_Alarms' named 'RTC_alarm'
liftRelayArray relayArray;	// Make a object of the 'class liftRelayArray' named 'relayArray'



Human_Machine_Interface::Human_Machine_Interface()
{
	UIstate = 0;
	time_t temp = 0;
	tid = HMI.ConvTotm(temp);
	tid.Hour = 8;
	tid.Minute = 30;
}

void Human_Machine_Interface::printDateTime(tmElements_t TM)
{
	Serial << ((TM.Day<10) ? "0" : "") << TM.Day << ' ';
	Serial << monthShortStr(TM.Month) << ' ';
	Serial << ((TM.Hour<10) ? "0" : "") << TM.Hour << ':';
	Serial << ((TM.Minute<10) ? "0" : "") << TM.Minute << ':';
	Serial << ((TM.Second<10) ? "0" : "") << TM.Second << endl;
}

void Human_Machine_Interface::printDateTime(time_t t)
{
	Serial << ((day(t)<10) ? "0" : "") << _DEC(day(t)) << ' ';
	Serial << monthShortStr(month(t)) << " " << _DEC(year(t)) << ' ';
	Serial << ((hour(t)<10) ? "0" : "") << _DEC(hour(t)) << ':';
	Serial << ((minute(t)<10) ? "0" : "") << _DEC(minute(t)) << ':';
	Serial << ((second(t)<10) ? "0" : "") << _DEC(second(t));
}

tmElements_t Human_Machine_Interface::ConvTotm(time_t t)
{
	// to convert back to time_t run makeTime
	tmElements_t TM;
	TM.Year = year(t);
	TM.Day = day(t);
	TM.Month = month(t);
	TM.Hour = hour(t);
	TM.Minute = minute(t);
	TM.Second = second(t);
	
	return TM;
}

uint8_t Human_Machine_Interface::read_LCD_buttons(void)
{
	// read the value from the sensor
	int adc_key_in = analogRead(btnPIN);
	
	if (adc_key_in > 1050) return btnRESET;
	if (adc_key_in < 50)   return btnRIGHT;
	if (adc_key_in < 250)  return btnUP;
	if (adc_key_in < 450)  return btnDOWN;
	if (adc_key_in < 650)  return btnLEFT;
	if (adc_key_in < 850)  return btnSELECT;
	
	return 0;                // when all others fail, return this.
}

void Human_Machine_Interface::UIupdate(void)
{
 	uint8_t userState = 0;
	if (UIdelay >= UIbtnHold)
	{
		UIdelay = 0;
		UIdelayStat = 0;

		userState = btnStat;	// insert user input here.
		btnStat = 0;
	}
	else
	{
		UIdelayStat = 1;
	}

//	userState = HMI.read_LCD_buttons();

	switch (UIstate)
	{
		case 0:
			// Update tid with time from DS3231
			tid = HMI.ConvTotm(RTC.get());

			// Print time here on LCD
			lcd.clear();
			lcd.setCursor(0,0);
			lcd << "Klokkeslaet";
			lcd.setCursor(0,1);
			lcd << ((tid.Hour<10) ? "0" : "") << tid.Hour << ":" << ((tid.Minute<10) ? "0" : "") << tid.Minute << "";

			// user input
			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ // Go to UIstate 1, To change C1
				UIstate = 1;

				break;
				case btnRESET:
				/* Your code here */ //Nothing happens
				break;
				case btnUP:
				/* Your code here */ //nothing happens
				break;
				case btnDOWN:
				/* Your code here */ //nothing happens
				break;
				case btnLEFT:
				/* Your code here */ //Go to UIstate 20, we go to alarm 2
				UIstate = 20;

				break;
				case btnRIGHT:
				/* Your code here */ //Go to UIstate 10, we go to alarm 1
				UIstate = 10;

				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 1:
		// Show time, blink C1
			lcd.clear();
			lcd.setCursor(0,0);
			lcd << "Skift Klokkeslet";
			lcd.setCursor(0,1);
			lcd << ((tid.Hour<10) ? "0" : "") << tid.Hour << ":" << ((tid.Minute<10) ? "0" : "") << tid.Minute << "";
			
			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ //Do nothing

				break;
				case btnRESET:
				/* Your code here */ //Do nothing

				break;
				case btnUP:
				/* Your code here */ //Add 10 to variable hours
				//If variable hours is 23< then subtract 20
				if ((0 <= tid.Hour) & (tid.Hour < 14))
				{
					tid.Hour += 10;
				} 
				else if((14 <= tid.Hour) & (tid.Hour < 20))
				{
					tid.Hour -= 10;
				}
				else
				{
					tid.Hour -= 20;
				}
				
				break;
				case btnDOWN:
				/* Your code here */ //subtract 10 to variable hours
				// If variable hours is <0 then add 20
				if ((0 <= tid.Hour) & (tid.Hour < 4))
				{
					tid.Hour += 20;
				}
				else if((4 <= tid.Hour) & (tid.Hour < 10))
				{
					tid.Hour += 10;
				}
				else
				{
					tid.Hour -= 10;
				}
				

				break;
				case btnLEFT:
				/* Your code here */ //Go to UIstate 0, revert changes, see time
				UIstate = 0;

				break;
				case btnRIGHT:
				/* Your code here */ //Go to UIstate 2, To change C2
				UIstate = 2;

				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 2:
			// Show time, blink C2
			lcd.clear();
			lcd.setCursor(0,0);
			lcd << "Skift Klokkeslet";
			lcd.setCursor(0,1);
			lcd << ((tid.Hour<10) ? "0" : "") << tid.Hour << ":" << ((tid.Minute<10) ? "0" : "") << tid.Minute << "";

			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ //Do nothing

				break;
				case btnRESET:
				/* Your code here */ //Do nothing

				break;
				case btnUP:
				/* Your code here */ //Add 1 to variable hours
				//If variable hours is 23< then subtract 3
				if ((9 == tid.Hour) | (19 == tid.Hour))
				{
					tid.Hour -= 9;
				}
				else if(23 == tid.Hour)
				{
					tid.Hour -= 3;
				}
				else
				{
					tid.Hour += 1;
				}

				break;
				case btnDOWN:
				/* Your code here */ //subtract 1 to variable hours
				// If variable hours is <0 then add 9
				if ((0 == tid.Hour) | (10 == tid.Hour))
				{
					tid.Hour += 9;
				}
				else if(20 == tid.Hour)
				{
					tid.Hour += 3;
				}
				else
				{
					tid.Hour -= 1;
				}

				break;
				case btnLEFT:
				/* Your code here */ //Go to UIstate 1, to change C1
				UIstate = 1;

				break;
				case btnRIGHT:
				/* Your code here */ // Go to UIstate 3, To change C3
				UIstate = 3;

				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 3:
			// Show time, blink C3
			lcd.clear();
			lcd.setCursor(0,0);
			lcd << "Skift Klokkeslet";
			lcd.setCursor(0,1);
			lcd << ((tid.Hour<10) ? "0" : "") << tid.Hour << ":" << ((tid.Minute<10) ? "0" : "") << tid.Minute << "";

			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ // Do nothing

				break;
				case btnRESET:
				/* Your code here */ //Do nothing

				break;
				case btnUP:
				/* Your code here */ //Add 10 to variable minutes
				//If variable minutes is 59< then subtract 50
				if (tid.Hour >= 59)
				{
					tid.Hour =- 50;
				}
				else
				{
					tid.Hour =+ 10;
				}

				break;
				case btnDOWN:
				/* Your code here */ //subtract 10 to variable minutes
				// If variable minutes is <0 then add 50
				if (tid.Hour <= 0)
				{
					tid.Hour =+ 50;
				}
				else
				{
					tid.Hour =- 10;
				}

				break;
				case btnLEFT:
				/* Your code here */ //Go to UIstate 2, to change C2
				UIstate = 2;

				break;
				case btnRIGHT:
				/* Your code here */ //Go to UIstate 4, To change C4
				UIstate = 4;

				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 4:
			// Show time, blink C4
			lcd.clear();
			lcd.setCursor(0,0);
			lcd << "Skift Klokkeslet";
			lcd.setCursor(0,1);

			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ // Go to UIstate 0, write time to timer module, 
				UIstate = 0;
				RTC.set(makeTime(tid));

				break;
				case btnRESET:
				/* Your code here */ //Do nothing

				break;
				case btnUP:
				/* Your code here */ //Add 1 to variable minutes
				//If variable minutes is 59< then subtract 9
				if (tid.Hour >= 59)
				{
					tid.Hour =- 9;
				}
				else
				{
					tid.Hour =+ 1;
				}

				break;
				case btnDOWN:
				/* Your code here */ //subtract 1 to variable minutes
				// If variable minutes is >0 then add 9
				if (tid.Hour <= 0)
				{
					tid.Hour =+ 9;
				}
				else
				{
					tid.Hour =- 1;
				}

				break;
				case btnLEFT:
				/* Your code here */ //Go to UIstate 3, to change C3
				UIstate = 3;

				break;
				case btnRIGHT:
				/* Your code here */ //Do nothing

				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 10:
			// Show Alarm1
			lcd.clear();
			lcd.setCursor(0,0);
			lcd << "Doeren aabner:";
			lcd.setCursor(0,1);
			

			tid = HMI.ConvTotm(RTC_alarm.alarm1_get());
			lcd << ((tid.Hour<10) ? "0" : "") << tid.Hour << ":" << ((tid.Minute<10) ? "0" : "") << tid.Minute << "";

			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ // Go to UIstate 11, to change C1
				UIstate = 11;

				break;
				case btnRESET:
				/* Your code here */ //Do nothing

				break;
				case btnUP:
				/* Your code here */ //Do nothing

				break;
				case btnDOWN:
				/* Your code here */ //Do nothing

				break;
				case btnLEFT:
				/* Your code here */ //Go to UIstate 0, we go to Time
				UIstate = 0;

				break;
				case btnRIGHT:
				/* Your code here */ //Go to UIstate 20, we go to Alarm2
				UIstate = 20;

				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 11:
			// Show Alarm1, blink C1
			lcd.clear();
			lcd.setCursor(0,0);
			lcd << "Skift Abningstid";
			lcd.setCursor(0,1);

			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ //Do nothing

				break;
				case btnRESET:
				/* Your code here */ //Do nothing
				break;
				case btnUP:
				/* Your code here */ //Add 10 to variable hours
				//If variable hours is 23< then subtract 20
				if (tid.Hour >= 24)
				{
					tid.Hour =- 20;
				}
				else
				{
					tid.Hour =+ 10;
				}

				break;
				case btnDOWN:
				/* Your code here */ //subtract 10 to variable hours
				// If variable hours is <0 then add 20
				if (tid.Hour <= 0)
				{
					tid.Hour =+ 20;
				}
				else
				{
					tid.Hour =- 10;
				}

				break;
				case btnLEFT:
				/* Your code here */ //Go to UIstate 10, revert changes, see alarm1
				UIstate = 10;

				break;
				case btnRIGHT:
				/* Your code here */ // Go to UIstate 12, To change C2
				UIstate = 12;

				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 12:
			// Show Alarm1, blink C2
			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ //Do nothing
				break;
				case btnRESET:
				/* Your code here */ //Do nothing
				break;
				case btnUP:
				/* Your code here */ //Add 1 to variable hours
				//If variable hours is 23< then subtract 3
				if (tid.Hour <= 0)
				{
					tid.Hour =+ 9;
				}
				else
				{
					tid.Hour =- 1;
				}

				break;
				case btnDOWN:
				/* Your code here */ //subtract 1 to variable hours
				// If variable hours is >0 then add 9
				if (tid.Hour <= 0)
				{
					tid.Hour =+ 9;
				}
				else
				{
					tid.Hour =- 1;
				}

				break;
				case btnLEFT:
				/* Your code here */ //Go to UIstate 11, To change C1
				UIstate = 11;

				break;
				case btnRIGHT:
				/* Your code here */ // Go to UIstate 13, To change C3
				UIstate = 13;

				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 13:
			// Show Alarm1, blink C3
			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ // Go to UIstate 14, To change C4
				break;
				case btnRESET:
				/* Your code here */ //Go to UIstate 12, To change C2
				break;
				case btnUP:
				/* Your code here */ //Add 10 to variable minutes
				//If variable hours is 59< then subtract 50
				break;
				case btnDOWN:
				/* Your code here */ //subtract 10 to variable minutes
				// If variable hours is >0 then add 50
				break;
				case btnLEFT:
				/* Your code here */ //Do nothing
				break;
				case btnRIGHT:
				/* Your code here */ //Do nothing
				break;
				default:
				/* Your code here */
			break;
			}
		break;
		case 14:
			// Show Alarm1, blink C4
			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ // Go to UIstate 10, Write Alarm1 to timer module
				RTC_alarm.alarm1_set(tid);

				break;
				case btnRESET:
				/* Your code here */ //Go to UIstate 13, To change C3
				break;
				case btnUP:
				/* Your code here */ //Add 1 to variable minutes
				//If variable hours is 59< then subtract 9
				break;
				case btnDOWN:
				/* Your code here */ //subtract 1 to variable minutes
				// If variable hours is >0 then add 9
				break;
				case btnLEFT:
				/* Your code here */ //Do nothing
				break;
				case btnRIGHT:
				/* Your code here */ //Do nothing
				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 20:
			// Show Alarm2
			lcd.clear();
			lcd.setCursor(0,0);
			lcd << "Doeren Lukker:";
			lcd.setCursor(0,1);
			

			tid = HMI.ConvTotm(RTC_alarm.alarm2_get());
			lcd << ((tid.Hour<10) ? "0" : "") << tid.Hour << ":" << ((tid.Minute<10) ? "0" : "") << tid.Minute << "";

			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ // Go to UIstate 21, To change C1
				break;
				case btnRESET:
				/* Your code here */ //Do nothing
				break;
				case btnUP:
				/* Your code here */ //Do nothing
				break;
				case btnDOWN:
				/* Your code here */ //Do nothing
				break;
				case btnLEFT:
				/* Your code here */ //Go to UIstate 10, we go to Alarm1
				UIstate = 10;

				break;
				case btnRIGHT:
				/* Your code here */ //Go to UIstate 0, we go to Time
				UIstate = 0;

				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 21:
			// Show Alarm2, blink C1
			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ // Go to UIstate 22, To change C2
				break;
				case btnRESET:
				/* Your code here */ //Go to UIstate 20, revert changes, see Alarm2
				break;
				case btnUP:
				/* Your code here */ //Add 10 to variable hours
				//If variable hours is 23< then subtract 20
				break;
				case btnDOWN:
				/* Your code here */ //subtract 10 to variable hours
				// If variable hours is >0 then add 20
				break;
				case btnLEFT:
				/* Your code here */ //Do nothing
				break;
				case btnRIGHT:
				/* Your code here */ //Do nothing
				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 22:
			// Show Alarm2, blink C2
			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ // Go to UIstate 23, To change C3
				break;
				case btnRESET:
				/* Your code here */ //Go to UIstate 21, To change C1
				break;
				case btnUP:
				/* Your code here */ //Add 1 to variable hours
				//If variable hours is 23< then subtract 3
				break;
				case btnDOWN:
				/* Your code here */ //subtract 1 to variable hours
				// If variable hours is >0 then add 9
				break;
				case btnLEFT:
				/* Your code here */ //Do nothing
				break;
				case btnRIGHT:
				/* Your code here */ //Do nothing
				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 23:
			// Show Alarm2, blink C3
			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ // Go to UIstate 24, To change C4
				break;
				case btnRESET:
				/* Your code here */ //Go to UIstate 22, To change C2
				break;
				case btnUP:
				/* Your code here */ //Add 10 to variable minutes
				//If variable hours is 59< then subtract 50
				break;
				case btnDOWN:
				/* Your code here */ //subtract 10 to variable minutes
				// If variable hours is >0 then add 50
				break;
				case btnLEFT:
				/* Your code here */ //Do nothing
				break;
				case btnRIGHT:
				/* Your code here */ //Do nothing
				break;
				default:
				/* Your code here */
				break;
			}
		break;
		case 24:
			// Show Alarm2, blink C4
			switch (userState)
			{
				case btnSELECT:
				/* Your code here */ // Go to UIstate 23, To change C3
				break;
				case btnRESET:
				/* Your code here */ //Go to UIstate 20, write Alarm2 to timer module
				break;
				case btnUP:
				/* Your code here */ //Add 1 to variable minutes
				//If variable hours is 59< then subtract 9
				break;
				case btnDOWN:
				/* Your code here */ //subtract 1 to variable minutes
				// If variable hours is >0 then add 9
				break;
				case btnLEFT:
				/* Your code here */ //Do nothing
				break;
				case btnRIGHT:
				/* Your code here */ //Do nothing
				break;
				default:
				/* Your code here */
				break;
			}
		break;
		default:	// default state, aka state 0.
			printDateTime(ConvTotm(RTC.get()));
			// user input
		break;
	}
}


DS3231RTC_Alarms::DS3231RTC_Alarms()
{
	alarm1_time.long_time = 0;
	alarm2_time.long_time = 0;
}

void DS3231RTC_Alarms::init_alarms(void)
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

	// Read the alarm time from the eeprom
	for (int i = 0; i < 7; i++)
	{
		DS3231RTC_Alarms::alarm1_time.byte_array[0+i] = eeprom_read_byte((uint8_t *)alarm1_addr+i);
	}

	for (int i = 0; i < 7; i++)
	{
		DS3231RTC_Alarms::alarm2_time.byte_array[0+i] = eeprom_read_byte((uint8_t *)alarm2_addr+i);
	}
}

void DS3231RTC_Alarms::alarm_Check(uint8_t *stat)
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

time_t DS3231RTC_Alarms::alarm1_get(void)
{
	return DS3231RTC_Alarms::alarm1_time.long_time;
}

time_t DS3231RTC_Alarms::alarm2_get(void)
{
	return DS3231RTC_Alarms::alarm2_time.long_time;
}

void DS3231RTC_Alarms::alarm1_set(tmElements_t TM)
{
	union {
		long int long_variable;
		byte byte_array[7];
	} u;

	RTC.setAlarm(ALM1_MATCH_HOURS, TM.Second, TM.Minute, TM.Hour, 1);		//daydate parameter should be between 1 and 7
	RTC.alarm(ALARM_1);														//ensure RTC interrupt flag is cleared
	RTC.alarmInterrupt(ALARM_1, true);

	u.long_variable = makeTime(TM);
	for (int i = 0; i < 7; i++)
	{
		eeprom_write_byte((uint8_t *)alarm1_addr+i, u.byte_array[0+i]);
	}
}

void DS3231RTC_Alarms::alarm2_set(tmElements_t TM)
{
	union {
		long int long_variable;
		byte byte_array[7];
	} u;

	RTC.setAlarm(ALM2_MATCH_HOURS, TM.Second, TM.Minute, TM.Hour, 1);		//daydate parameter should be between 1 and 7
	RTC.alarm(ALARM_2);														//ensure RTC interrupt flag is cleared
	RTC.alarmInterrupt(ALARM_2, true);

	u.long_variable = makeTime(TM);
	for (int i = 0; i < 7; i++)
	{
		eeprom_write_byte((uint8_t *)alarm2_addr+i, u.byte_array[0+i]);
	}
}


liftRelayArray::liftRelayArray()
{

}

void liftRelayArray::relayArrayInit(void)
{
	// Initialize Buttons
	DDRD |= (1 << RAControl1) | (1 << RAControl2) | (1 << RAControl3) | (1 << RAControl4);		// Marks pins as output.
	PORTD &= ~(1 << RAControl1) & ~(1 << RAControl2) & ~(1 << RAControl3) & ~(1 << RAControl4);	// Puts pins into off state.

	/*
	Use ports:
	PORTC |= (1 << DDC1);	// Make PC1 = 1 (on)
	PORTC &= ~(1 << DDC1);	// Puts PC1 = 0 (off).
	*/

	// setup timer1 to make an interrupt every 1 ms
	noInterrupts();           // disable all interrupts
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;

	OCR1A = 16000;            // compare match register 16MHz/1000
	TCCR1B |= (1 << WGM12);   // CTC mode
	TCCR1B |= (1 << CS10);    // No prescaler
	TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
	interrupts();             // enable all interrupts
}

void liftRelayArray::relayArrayCommand(uint8_t cmd)
{
	switch (cmd)
	{
		case liftCW:
			PORTD |= (1 << RAControl1);
			PORTD &= ~(1 << RAControl2);
			PORTD |= (1 << RAControl3);
		break;
		case liftCCW:
			PORTD |= (1 << RAControl1);
			PORTD |= (1 << RAControl2);
			PORTD |= (1 << RAControl3);
		break;
		default:	// default, aka. liftSTOP
			PORTD &= ~(1 << RAControl1);
			PORTD &= ~(1 << RAControl2);
			PORTD &= ~(1 << RAControl3);
		break;
	}
}

void liftRelayArray::relayAutoCommand(boolean alarm1trig, boolean alarm2trig)
{
	if (alarm1trig == 1)		// if alarm 1 was triggered:
	{
		// change direction if necessary
		relayArrayCommand(liftCW);
		RACounter1 = 0;			// Reset RACounter1.
		RACounter1Status = 1;	// start timer and stop after x seconds (see defines)
	} 
	else if (alarm2trig == 1)	// if alarm 2 was triggered:
	{
		// change direction if necessary
		relayArrayCommand(liftCCW);
		RACounter1 = 0;			// Reset RACounter1.
		RACounter1Status = 1;	// start timer and stop after x seconds (see defines)
	}
	else						// if none of the alarms were triggered:
	{
		if (RACounter1 >= RAHold)
		{
			relayArrayCommand(liftSTOP);
			RACounter1Status = 0;
		}
	}
}


ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
	// Debugging
 	if (T1Timer >= 500)
 	{
 		test++;
 		T1Timer = 0;
 
 		Serial << "Test is: " << test << endl;
 	}
 	else
 	{
 		T1Timer++;
 	}

	// HMI interface timers:
	if (!btnStat)
	{
		btnStat = HMI.read_LCD_buttons();
	}

	if (UIdelayStat == 1)
	{
		UIdelay++;
	}
	
	// RelayArray:
	if (RACounter1Status == 1)
	{
		RACounter1++;
	}
}

#endif