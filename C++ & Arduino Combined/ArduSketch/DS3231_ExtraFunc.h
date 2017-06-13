/*
 * DS3231_ExtraFunc.h
 *
 * Created: 21-04-2017 19:11:26
 *  Author: Hans V. Rasmussen
 */ 

/* 
	This library contains the functions of Marco Collis MD_DS3231_Test sketch for Arduino.
	The only thing I have done is making it a library and shlightly fixing some errors for use with C++.
*/

#ifndef DS3231_EXTRAFUNC_H_
#define DS3231_EXTRAFUNC_H_

#define PRINTS(s)   Serial.print(F(s))
#define PRINT(s, v) { Serial.print(F(s)); Serial.print(v); }


void cbAlarm2(); // callback function for Alarm 2
void cbAlarm1(); // callback function for Alarm 2
char *p2dig(uint8_t v, uint8_t mode); // 2 digits leading zero
uint8_t i2dig(uint8_t mode); // input 2 digits in the specified base
void usage(void);
char *dow2String(uint8_t code);
char *ctl2String(codeRequest_t code);
char *sts2String(codeStatus_t code, boolean bHexValue = false);
char *alm2String(almType_t alm);
uint8_t htoi(char c);
void showStatus();
void printTime();
void showTime();
void showAlarm1();
void showAlarm2();
void showRAM();
void inputTime(void);
void showDoW(void);
void doAlarm1();
void doAlarm2();
void writeControl();
char readNext(); /* Read the next character from the serial input stream, skip whitespace. */ // Busy loop that also checks for an alarm occurence.
void Debug(boolean *contDebug);


#endif /* DS3231_EXTRAFUNC_H_ */