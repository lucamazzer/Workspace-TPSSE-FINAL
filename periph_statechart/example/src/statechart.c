/*
 * @brief State Configurable Timer (SCT) PWM example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
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
//#include "funciones.h"
/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define BOARD_NXP_LPCXPRESSO_4337

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
static volatile bool fAlarmTimeMatched;
volatile bool ServoFlag=false;
static uint32_t tick_ct = 0;
static volatile bool fAlarmTimeMatched;

typedef struct{

	RTC_TIME_T clock;
	uint8_t food;
	bool init;

}food_t;
typedef enum hora{mayor, menor, ordenado }status_time;

#define SCT_PWM_RATE   50        /* PWM frequency 50 Hz */

/* Systick timer tick rate, to change duty cycle */
#define TICKRATE_HZ     100        /* 1 ms Tick rate */
#define DUTY_CYCLE 90/100	//Duty Cycle configurado 180 grados
#define DUTY_CYCLE_full 98/100	//Duty Cycle configurado 0 grados
#define MAX_POS 3


/*****************************************************************************
 * Private functions
 ****************************************************************************/


/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * *
 *  * * *  FUNCIONES PARA EL SERVO Y PWM* * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * *  * * * * * * * * * */

void toggle_servo (void){

	if (ServoFlag== false) Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_GetTicksPerCycle(LPC_SCT)*DUTY_CYCLE_full );
	if (ServoFlag== true) Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_GetTicksPerCycle(LPC_SCT)*DUTY_CYCLE );
}

void toggle_flag(void){

	if(ServoFlag== false) ServoFlag=true;
	else ServoFlag=false;
}


/* * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * *
 *  * * * * FUNCIONES PARA EL RTC* * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * *  * * * * * * * * * */

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
	//RTC_TIME_T Alarm;
	RTC_TIME_T FullTime;

	Chip_RTC_GetFullTime(LPC_RTC, &FullTime);

	if(pos<MAX_POS){
	    FullTime.time[RTC_TIMETYPE_SECOND]  = (AlarmVector[pos].clock).time[RTC_TIMETYPE_SECOND];
		FullTime.time[RTC_TIMETYPE_MINUTE]  = (AlarmVector[pos].clock).time[RTC_TIMETYPE_MINUTE];
		FullTime.time[RTC_TIMETYPE_HOUR]    = (AlarmVector[pos].clock).time[RTC_TIMETYPE_HOUR];
		/*Alarm.time[RTC_TIMETYPE_DAYOFMONTH]  = (AlarmVector[pos].clock).time[RTC_TIMETYPE_DAYOFMONTH];
		Alarm.time[RTC_TIMETYPE_DAYOFWEEK]  = (AlarmVector[pos].clock).time[RTC_TIMETYPE_DAYOFWEEK];
		Alarm.time[RTC_TIMETYPE_DAYOFYEAR]    = (AlarmVector[pos].clock).time[RTC_TIMETYPE_DAYOFYEAR];
		Alarm.time[RTC_TIMETYPE_MONTH]    = (AlarmVector[pos].clock).time[RTC_TIMETYPE_MONTH];
		Alarm.time[RTC_TIMETYPE_YEAR]    = (AlarmVector[pos].clock).time[RTC_TIMETYPE_YEAR];*/

		Chip_RTC_SetFullAlarmTime(LPC_RTC, &FullTime);
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


/*MAYOR ES QUE VIENE DESPUES */
status_time Check_Time(RTC_TIME_T time1 , RTC_TIME_T time2) // chequea si time1 es mayor o menor que time2
{
	if (time1.time[RTC_TIMETYPE_HOUR]>time2.time[RTC_TIMETYPE_HOUR])
	{
		return mayor;
	}
	else
	{
		if(time1.time[RTC_TIMETYPE_HOUR]<time2.time[RTC_TIMETYPE_HOUR])
		{
			return menor;
		}
		else
		{
			if(time1.time[RTC_TIMETYPE_HOUR]==time2.time[RTC_TIMETYPE_HOUR])
			{
				if (time1.time[RTC_TIMETYPE_MINUTE]>time2.time[RTC_TIMETYPE_MINUTE])
				{
					return mayor;
				}
				else
				{
					if(time1.time[RTC_TIMETYPE_MINUTE]<time2.time[RTC_TIMETYPE_MINUTE])
					{
						return menor;
					}
					else
					{
						if(time1.time[RTC_TIMETYPE_MINUTE]==time2.time[RTC_TIMETYPE_MINUTE])
						{
							if(time1.time[RTC_TIMETYPE_SECOND]<time2.time[RTC_TIMETYPE_SECOND])
							{
								return menor;
							}
							else return mayor;
						}
					}
				}
			}
		}
	}
}


status_time VectorAlarmSort(food_t * VectorFood)
{
	uint8_t ordenado=0;
	uint8_t i;
	uint8_t j;

	for(i=0;i<MAX_POS -1 && ordenado ==0; i++)
	{	ordenado=1;
		for(j=0;j< MAX_POS-(i+1);j++)
		{
			if(Check_Time(VectorFood[j].clock,VectorFood[j+1].clock)== mayor)
			{
				swap(VectorFood,j,j+1);
				ordenado=0;
			}
		}
	}

}

/*  NO SIRVEPOR AHORA ES MEJOR ORDENAR EL VECTOR D EMENOR A MAYOR Y ver si cada uno paso
 *
void Set_next_Alarm(food_t * FoodVector){

	RTC_TIME_T FullTime;
	Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
	uint8_t NextAlarm=0;
	uint8_t pivot=0;

	if(Check_Time(VectorFood[Pivot].clock,FullTime)== mayor)// checkeo si el q esta en la posicion cero todavia no paso
	{
		NextAlarm=0;
		pivot++;
		if(Check_Time(VectorFood[NextAlarm].clock,VectorFood[Pivot].clock)== mayor)// si laproxima alarma esta dps de la del pivot
		{

		}

	}
	else
	{
		pivot++;
		if(Check_Time(VectorFood[Pivot].clock,FullTime)== mayor)// checkeo si el q esta en la posicion 1 todavia no paso
		{

		}
		else
		{	pivot++;
			if(Check_Time(VectorFood[Pivot].clock,FullTime)== mayor) // checkeo si el 3ro no paso (si ninguno paso pongo el menor!)
			{

			}

		}
	}

}
*/


void ServirComida (food_t * Comida, uint8_t * position){
	uint8_t i;
	Chip_SCTPWM_Start(LPC_SCT);
	for(i=0;i<3;i++){
		toggle_servo ();
		tick_ct=0;
		while(tick_ct<150){
			__WFI();
		}
	}
	Chip_SCTPWM_Stop(LPC_SCT);
	if(*position<MAX_POS){
	(*position)++;
	}
	else *position=0;
	set_new_alarm(Comida, *position);

}

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * *
 *  * * * * HANDLES DE INTERRUPCIONES * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * *  * * * * * * * * * */



void SysTick_Handler(void)
{
	/* This is only used to wakeup up the device from sleep */

	tick_ct += 1;
	if ((tick_ct % 150) == 0) toggle_flag();

}
void SCT_IRQHandler(void){

}




void RTC_IRQHandler(void)
{
	/* Check for alarm match */
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM)) {
		/* Clear pending interrupt */
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);
		fAlarmTimeMatched = true;	/* set alarm handler flag */
	}
}



int main(void)
{
	/* Generic Initialization */
	fAlarmTimeMatched = 0;
	RTC_TIME_T FullTime;
	food_t VectorFood[MAX_POS];
	uint8_t pos=0;
	SystemCoreClockUpdate();
	Board_Init();
	//uint8_t i;

	/* Initialize the SCT as PWM and set frequency */
	Chip_SCTPWM_Init(LPC_SCT);
	Chip_SCTPWM_SetRate(LPC_SCT, SCT_PWM_RATE);

	/* Setup Board specific output pin */
	Chip_SCU_PinMuxSet(0x4, 4, (SCU_MODE_INACT | SCU_MODE_FUNC1));	//CTOUT2=P4_4

	/* Use SCT0_OUT1 pin */
	Chip_SCTPWM_SetOutPin(LPC_SCT, 1, 2);

	/* Start with 0% duty cycle */
	//Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_GetTicksPerCycle(LPC_SCT)*DUTY_CYCLE );
	//Chip_SCTPWM_Start(LPC_SCT);
	//Chip_SCT_EnableEventInt(LPC_SCT, SCT_EVT_1);

	/* Enable SysTick Timer */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);

	/*CONFIGURACION REAL CLOCK*/


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

	/*inicializo el vector de comida con el horario del momento!*/
	VectorFoodInit(VectorFood, FullTime);

	/* Set ALARM time */

	VectorFood[0].clock.time[RTC_TIMETYPE_SECOND]  = 15; // prueba d ealarma
	VectorFood[1].clock.time[RTC_TIMETYPE_SECOND]  = 30;
	VectorFood[2].clock.time[RTC_TIMETYPE_SECOND]  = 45;

	Chip_RTC_SetFullAlarmTime(LPC_RTC, &(VectorFood[0].clock));

	/* Enable matching for alarm for second, minute, hour fields only */
	Chip_RTC_AlarmIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC | RTC_AMR_CIIR_IMMIN | RTC_AMR_CIIR_IMHOUR, ENABLE);

	/* Clear interrupt pending */
	Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE | RTC_INT_ALARM);

	/* Enable RTC interrupt in NVIC */
	NVIC_EnableIRQ((IRQn_Type) RTC_IRQn);

	/* Enable RTC (starts increase the tick counter and second counter register) */
	Chip_RTC_Enable(LPC_RTC, ENABLE);




	while (1) {
		__WFI();
		if(fAlarmTimeMatched){
			fAlarmTimeMatched = false;
			ServirComida(VectorFood,&pos);
		}
	}
}
