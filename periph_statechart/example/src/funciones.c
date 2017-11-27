/*
 * @brief RTC example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "funciones.h"

static volatile bool fIntervalReached;
static volatile bool fAlarmTimeMatched;
static volatile bool On0, On1;
/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Gets and shows the current time and date */
static void showTime(RTC_TIME_T *pTime)
{
	DEBUGOUT("Time: %.2d:%.2d:%.2d %.2d/%.2d/%.4d\r\n", pTime->time[RTC_TIMETYPE_HOUR],
			 pTime->time[RTC_TIMETYPE_MINUTE],
			 pTime->time[RTC_TIMETYPE_SECOND],
			 pTime->time[RTC_TIMETYPE_MONTH],
			 pTime->time[RTC_TIMETYPE_DAYOFMONTH],
			 pTime->time[RTC_TIMETYPE_YEAR]);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/



void VectorFoodInit(food_t * VectorFood, RTC_TIME_T FullTime)
{ int i;

	for(i=0;i< MAX_POS; i++)
	{
		VectorFood[i].clock.time[RTC_TIMETYPE_SECOND]=FullTime.time[RTC_TIMETYPE_SECOND];
		VectorFood[i].clock.time[RTC_TIMETYPE_MINUTE]=FullTime.time[RTC_TIMETYPE_MINUTE];
		VectorFood[i].clock.time[RTC_TIMETYPE_HOUR]=FullTime.time[RTC_TIMETYPE_HOUR];
		VectorFood[i].clock.time[RTC_TIMETYPE_DAYOFMONTH]=FullTime.time[RTC_TIMETYPE_DAYOFMONTH];
		VectorFood[i].clock.time[RTC_TIMETYPE_DAYOFWEEK]=FullTime.time[RTC_TIMETYPE_DAYOFWEEK];
		VectorFood[i].clock.time[RTC_TIMETYPE_DAYOFYEAR]=FullTime.time[RTC_TIMETYPE_DAYOFYEAR];
		VectorFood[i].clock.time[RTC_TIMETYPE_MONTH]=FullTime.time[RTC_TIMETYPE_MONTH];
		VectorFood[i].clock.time[RTC_TIMETYPE_YEAR]=FullTime.time[RTC_TIMETYPE_YEAR];
		VectorFood[i].food=0;
		VectorFood[i].init=FALSE;
	}

}

void set_new_alarm(food_t * AlarmVector, uint8_t pos){
	RTC_TIME_T Alarm;

	if(pos<MAX_POS){
	    Alarm.time[RTC_TIMETYPE_SECOND]  = (AlarmVector[pos].clock).time[RTC_TIMETYPE_SECOND];
	    Alarm.time[RTC_TIMETYPE_MINUTE]  = (AlarmVector[pos].clock).time[RTC_TIMETYPE_MINUTE];
	    Alarm.time[RTC_TIMETYPE_HOUR]    = (AlarmVector[pos].clock).time[RTC_TIMETYPE_HOUR];
		Chip_RTC_SetFullAlarmTime(LPC_RTC, &Alarm);
	}
}
void swap(RTC_TIME_T * AlarmVector, uint8_t pos1,uint8_t pos2){


}

void VAlarm_ordenar(RTC_TIME_T * AlarmVector){

	uint8_t pos1=0;
	uint8_t pos2=1;
	uint8_t pos3=2;

	if ((AlarmVector[pos1]).time[RTC_TIMETYPE_HOUR] < (AlarmVector[pos2]).time[RTC_TIMETYPE_HOUR] )
	{
		swap(AlarmVector,pos1,pos2);
	}
	else
	{

		if ((AlarmVector[pos1]).time[RTC_TIMETYPE_HOUR] > (AlarmVector[pos2]).time[RTC_TIMETYPE_HOUR] )
		{
			swap(AlarmVector,pos2,pos1);
		}
		else
		{
			if ((AlarmVector[pos1]).time[RTC_TIMETYPE_MINUTE] < (AlarmVector[pos2]).time[RTC_TIMETYPE_MINUTE] )
			{
					swap(AlarmVector,pos1,pos2);
			}
			else
			{
				if ((AlarmVector[pos1]).time[RTC_TIMETYPE_MINUTE] > (AlarmVector[pos2]).time[RTC_TIMETYPE_MINUTE] )
				{
					swap(AlarmVector,pos2,pos1);
				}
			}

		}
	}
	// swap 2 con 3

	if ((AlarmVector[pos3]).time[RTC_TIMETYPE_HOUR] < (AlarmVector[pos2]).time[RTC_TIMETYPE_HOUR] )
	{
				swap(AlarmVector,pos3,pos2);
	}
	else
	{

		if ((AlarmVector[pos3]).time[RTC_TIMETYPE_HOUR] > (AlarmVector[pos2]).time[RTC_TIMETYPE_HOUR] )
		{
			swap(AlarmVector,pos2,pos3);
		}
		else
		{
			if ((AlarmVector[pos3]).time[RTC_TIMETYPE_MINUTE] < (AlarmVector[pos2]).time[RTC_TIMETYPE_MINUTE] )
			{
				swap(AlarmVector,pos3,pos2);
			}
			else
			{
				if ((AlarmVector[pos3]).time[RTC_TIMETYPE_MINUTE] > (AlarmVector[pos2]).time[RTC_TIMETYPE_MINUTE] )
				{
					swap(AlarmVector,pos2,pos3);
				}
			}

		}

     }
}

/**
 * @brief	Main entry point
 * @return	Nothing
 */

/* EJEMPLO DE USO DE RTC*/
int RTC(void)
{
	RTC_TIME_T FullTime;
//	RTC_TIME_T AlarmVector [3];
	SystemCoreClockUpdate();
	Board_Init();

	fIntervalReached  = 0;
	fAlarmTimeMatched = 0;
	//On0 = On1 = false;
	Board_LED_Set(2, false);

	/*DEBUGSTR("The RTC operates on a 1 Hz clock.\r\n" \
			 "Register writes can take up to 2 cycles.\r\n"	\
			 "It will take a few seconds to fully\r\n" \
			 "initialize it and start it running.\r\n\r\n");

//	DEBUGSTR("We'll print a timestamp every 5 seconds.\r\n"	\
			 "...and another when the alarm occurs.\r\n");
*/
	Chip_RTC_Init(LPC_RTC);

	/* Set current time for RTC 2:00:00PM, 2017-11-22 */
	FullTime.time[RTC_TIMETYPE_SECOND]  = 0;
	FullTime.time[RTC_TIMETYPE_MINUTE]  = 0;
	FullTime.time[RTC_TIMETYPE_HOUR]    = 14;
	FullTime.time[RTC_TIMETYPE_DAYOFMONTH]  = 22;
	FullTime.time[RTC_TIMETYPE_DAYOFWEEK]   = 3;
	FullTime.time[RTC_TIMETYPE_DAYOFYEAR]   = 326;
	FullTime.time[RTC_TIMETYPE_MONTH]   = 11;
	FullTime.time[RTC_TIMETYPE_YEAR]    = 2017;

	Chip_RTC_SetFullTime(LPC_RTC, &FullTime);

	/* Set ALARM time for 9:30:00 am */
	FullTime.time[RTC_TIMETYPE_SECOND]  = 0;
	FullTime.time[RTC_TIMETYPE_MINUTE]  = 30;
	FullTime.time[RTC_TIMETYPE_HOUR]    = 9;
	Chip_RTC_SetFullAlarmTime(LPC_RTC, &FullTime);

	/* Set the RTC to generate an interrupt on each second */
	Chip_RTC_CntIncrIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC, ENABLE);

	/* Enable matching for alarm for second, minute, hour fields only */
	Chip_RTC_AlarmIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC | RTC_AMR_CIIR_IMMIN | RTC_AMR_CIIR_IMHOUR, ENABLE);

	/* Clear interrupt pending */
	Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE | RTC_INT_ALARM);

	/* Enable RTC interrupt in NVIC */
	NVIC_EnableIRQ((IRQn_Type) RTC_IRQn);

	/* Enable RTC (starts increase the tick counter and second counter register) */
	Chip_RTC_Enable(LPC_RTC, ENABLE);

	/* Loop forever */
	while (1) {
		if (fIntervalReached) {	/* Every 5s */
			fIntervalReached = 0;

			On1 = (bool) !On1;
			Board_LED_Set(1, On1);

			/* read and display time */
			Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
			showTime(&FullTime);
		}

		if (fAlarmTimeMatched) {
			fAlarmTimeMatched = false;

			/* announce event */
			DEBUGSTR("ALARM triggered!\r\n");
			Board_LED_Set(2, true);

			/* read and display time */
			Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
			showTime(&FullTime);
		}
	}
}
