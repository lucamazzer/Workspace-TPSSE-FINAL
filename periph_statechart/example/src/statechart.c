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

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/


#define BOARD_NXP_LPCXPRESSO_4337

#define SCT_PWM_RATE   50        /* PWM frequency 10 KHz */

/* Systick timer tick rate, to change duty cycle */
#define TICKRATE_HZ     100        /* 1 ms Tick rate */

#define DUTY_CYCLE 90/100	//Duty Cycle configurado
#define DUTY_CYCLE_full 98/100	//Duty Cycle configurado
/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
volatile bool ServoFlag=false;
static uint32_t tick_ct = 0;
static volatile bool fAlarmTimeMatched;
/*****************************************************************************
 * Private functions
 ****************************************************************************/


/*****************************************************************************
 * Public functions
 ****************************************************************************/
void toggle_servo (void){

	if (ServoFlag== false) Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_GetTicksPerCycle(LPC_SCT)*DUTY_CYCLE_full );
	if (ServoFlag== true) Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_GetTicksPerCycle(LPC_SCT)*DUTY_CYCLE );
}

void toggle_flag(void){

	if(ServoFlag== false) ServoFlag=true;
	else ServoFlag=false;
}

/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
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


/* Example entry point */


int main(void)
{
	/* Generic Initialization */
	fAlarmTimeMatched = 0;
	RTC_TIME_T FullTime;
	int i;
	SystemCoreClockUpdate();
	Board_Init();

	/* Initialize the SCT as PWM and set frequency */
	Chip_SCTPWM_Init(LPC_SCT);
	Chip_SCTPWM_SetRate(LPC_SCT, SCT_PWM_RATE);

	/* Setup Board specific output pin */
	Chip_SCU_PinMuxSet(0x4, 4, (SCU_MODE_INACT | SCU_MODE_FUNC1));	//CTOUT2=P4_4

	/* Use SCT0_OUT1 pin */
	Chip_SCTPWM_SetOutPin(LPC_SCT, 1, 2);

	/* Start with 0% duty cycle */
	Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_GetTicksPerCycle(LPC_SCT)*DUTY_CYCLE_full );
	Chip_SCTPWM_Start(LPC_SCT);

	//Chip_SCT_EnableEventInt(LPC_SCT, SCT_EVT_1);

	/* Enable SysTick Timer */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);

	/*CONFIGURACION REAL CLOCK*/
/*

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

	  /* Set ALARM time for 14:00:20 am */
		FullTime.time[RTC_TIMETYPE_SECOND]  = 5;
		/*FullTime.time[RTC_TIMETYPE_MINUTE]  = 30;*/
		/*FullTime.time[RTC_TIMETYPE_HOUR]    = 9;*/
		Chip_RTC_SetFullAlarmTime(LPC_RTC, &FullTime);


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
		//toggle_servo ();
		if(fAlarmTimeMatched){
			fAlarmTimeMatched = false;
			for(i=0;i<7;i++){
				toggle_servo ();
				tick_ct=0;
				while(tick_ct<150){
				__WFI();
				}
			}
		}
	}
}
