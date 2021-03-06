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
#include "string.h"
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

typedef struct{

	RTC_TIME_T clock;
	uint8_t food;
	bool init;
	bool nextday;

}food_t;
typedef enum hora{mayor, menor,Ordenado }status_time;

#define SCT_PWM_RATE   50        /* PWM frequency 50 Hz */

/* Systick timer tick rate, to change duty cycle */
#define TICKRATE_HZ     100        /* 1 ms Tick rate */
#define DUTY_CYCLE 90/100	//Duty Cycle configurado 180 grados
#define DUTY_CYCLE_full 98/100	//Duty Cycle configurado 0 grados
#define MAX_POS 3 // maxima cantidad de alarmas!
volatile bool USART3_IRQ_flag=false;
volatile bool USART2_IRQ_flag=false;
char buffer[100];



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

/*inicializa el vector de comida y horario no inicializa la comida x ahora*/

static void showTime(RTC_TIME_T *pTime)
{
	DEBUGOUT("Time: %.2d:%.2d:%.2d %.2d/%.2d/%.4d\r\n", pTime->time[RTC_TIMETYPE_HOUR],
			 pTime->time[RTC_TIMETYPE_MINUTE],
			 pTime->time[RTC_TIMETYPE_SECOND],
			 pTime->time[RTC_TIMETYPE_MONTH],
			 pTime->time[RTC_TIMETYPE_DAYOFMONTH],
			 pTime->time[RTC_TIMETYPE_YEAR]);
}



void VectorFoodInit(food_t * VectorFood, RTC_TIME_T * FullTime)
{ int i;

	for(i=0;i< MAX_POS; i++)
	{
		VectorFood[i].clock.time[RTC_TIMETYPE_SECOND]=FullTime->time[RTC_TIMETYPE_SECOND];
		VectorFood[i].clock.time[RTC_TIMETYPE_MINUTE]=FullTime->time[RTC_TIMETYPE_MINUTE];
		VectorFood[i].clock.time[RTC_TIMETYPE_HOUR]=FullTime->time[RTC_TIMETYPE_HOUR];
		VectorFood[i].clock.time[RTC_TIMETYPE_DAYOFMONTH]=FullTime->time[RTC_TIMETYPE_DAYOFMONTH];
		VectorFood[i].clock.time[RTC_TIMETYPE_DAYOFWEEK]=FullTime->time[RTC_TIMETYPE_DAYOFWEEK];
		VectorFood[i].clock.time[RTC_TIMETYPE_DAYOFYEAR]=FullTime->time[RTC_TIMETYPE_DAYOFYEAR];
		VectorFood[i].clock.time[RTC_TIMETYPE_MONTH]=FullTime->time[RTC_TIMETYPE_MONTH];
		VectorFood[i].clock.time[RTC_TIMETYPE_YEAR]=FullTime->time[RTC_TIMETYPE_YEAR];
		VectorFood[i].food=0;
		VectorFood[i].init=FALSE;
		VectorFood[i].nextday=FALSE;
	}

}


/* setea una nueva alarma segun la posicion del vector que se le pase*/
/* tengo que modificarla para el caso en el que se pase de dia*/

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


/*swapea dos posiciones del vector copiando paso a paso*/
void swap(food_t * VectorFood, uint8_t pos1,uint8_t pos2){

	food_t aux;

	aux.clock.time[RTC_TIMETYPE_SECOND]  = (VectorFood[pos2].clock).time[RTC_TIMETYPE_SECOND];
	aux.clock.time[RTC_TIMETYPE_MINUTE]  = (VectorFood[pos2].clock).time[RTC_TIMETYPE_MINUTE];
	aux.clock.time[RTC_TIMETYPE_HOUR]    = (VectorFood[pos2].clock).time[RTC_TIMETYPE_HOUR];
	aux.clock.time[RTC_TIMETYPE_DAYOFMONTH]  = (VectorFood[pos2].clock).time[RTC_TIMETYPE_DAYOFMONTH];
	aux.clock.time[RTC_TIMETYPE_DAYOFWEEK]  = (VectorFood[pos2].clock).time[RTC_TIMETYPE_DAYOFWEEK];
	aux.clock.time[RTC_TIMETYPE_DAYOFYEAR]    = (VectorFood[pos2].clock).time[RTC_TIMETYPE_DAYOFYEAR];
	aux.clock.time[RTC_TIMETYPE_MONTH]    = (VectorFood[pos2].clock).time[RTC_TIMETYPE_MONTH];
	aux.clock.time[RTC_TIMETYPE_YEAR]    = (VectorFood[pos2].clock).time[RTC_TIMETYPE_YEAR];
	aux.food=VectorFood[pos2].food;

	VectorFood[pos2].clock.time[RTC_TIMETYPE_SECOND]  = (VectorFood[pos1].clock).time[RTC_TIMETYPE_SECOND];
	VectorFood[pos2].clock.time[RTC_TIMETYPE_MINUTE]  = (VectorFood[pos1].clock).time[RTC_TIMETYPE_MINUTE];
	VectorFood[pos2].clock.time[RTC_TIMETYPE_HOUR]    = (VectorFood[pos1].clock).time[RTC_TIMETYPE_HOUR];
	VectorFood[pos2].clock.time[RTC_TIMETYPE_DAYOFMONTH]  = (VectorFood[pos1].clock).time[RTC_TIMETYPE_DAYOFMONTH];
	VectorFood[pos2].clock.time[RTC_TIMETYPE_DAYOFWEEK]  = (VectorFood[pos1].clock).time[RTC_TIMETYPE_DAYOFWEEK];
	VectorFood[pos2].clock.time[RTC_TIMETYPE_DAYOFYEAR]    = (VectorFood[pos1].clock).time[RTC_TIMETYPE_DAYOFYEAR];
	VectorFood[pos2].clock.time[RTC_TIMETYPE_MONTH]    = (VectorFood[pos1].clock).time[RTC_TIMETYPE_MONTH];
	VectorFood[pos2].clock.time[RTC_TIMETYPE_YEAR]    = (VectorFood[pos1].clock).time[RTC_TIMETYPE_YEAR];
	VectorFood[pos2].food=VectorFood[pos1].food;

	VectorFood[pos1].clock.time[RTC_TIMETYPE_SECOND]  = aux.clock.time[RTC_TIMETYPE_SECOND];
	VectorFood[pos1].clock.time[RTC_TIMETYPE_MINUTE]  = aux.clock.time[RTC_TIMETYPE_MINUTE];
	VectorFood[pos1].clock.time[RTC_TIMETYPE_HOUR]    = aux.clock.time[RTC_TIMETYPE_HOUR];
	VectorFood[pos1].clock.time[RTC_TIMETYPE_DAYOFMONTH]  = aux.clock.time[RTC_TIMETYPE_DAYOFMONTH];
	VectorFood[pos1].clock.time[RTC_TIMETYPE_DAYOFWEEK]  = aux.clock.time[RTC_TIMETYPE_DAYOFWEEK];
	VectorFood[pos1].clock.time[RTC_TIMETYPE_DAYOFYEAR]    =aux.clock.time[RTC_TIMETYPE_DAYOFYEAR];
	VectorFood[pos1].clock.time[RTC_TIMETYPE_MONTH]    =aux.clock.time[RTC_TIMETYPE_MONTH];
	VectorFood[pos1].clock.time[RTC_TIMETYPE_YEAR]    = aux.clock.time[RTC_TIMETYPE_YEAR];
	VectorFood[pos1].food=aux.food;
}

/*MAYOR ES QUE VIENE DESPUES */
status_time Check_Time(RTC_TIME_T * time1 , RTC_TIME_T * time2) // chequea si time1 es mayor o menor que time2
{
	if (time1->time[RTC_TIMETYPE_HOUR]>time2->time[RTC_TIMETYPE_HOUR])
	{
		return mayor;
	}
	else
	{
		if(time1->time[RTC_TIMETYPE_HOUR]<time2->time[RTC_TIMETYPE_HOUR])
		{
			return menor;
		}
		else
		{
			if(time1->time[RTC_TIMETYPE_HOUR]==time2->time[RTC_TIMETYPE_HOUR])
			{
				if (time1->time[RTC_TIMETYPE_MINUTE]>time2->time[RTC_TIMETYPE_MINUTE])
				{
					return mayor;
				}
				else
				{
					if(time1->time[RTC_TIMETYPE_MINUTE]<time2->time[RTC_TIMETYPE_MINUTE])
					{
						return menor;
					}
					else
					{
						if(time1->time[RTC_TIMETYPE_MINUTE]==time2->time[RTC_TIMETYPE_MINUTE])
						{
							if(time1->time[RTC_TIMETYPE_SECOND]<time2->time[RTC_TIMETYPE_SECOND])
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
			if(Check_Time(&VectorFood[j].clock,&VectorFood[j+1].clock)== mayor)
			{
				swap(VectorFood,j,j+1);
				ordenado=0;
			}
		}
	}
	return Ordenado;

}

void ServirComida (food_t * Comida, uint8_t * position){
	uint8_t i;
	uint8_t cantidad;
	Chip_SCTPWM_Start(LPC_SCT);
	cantidad= Comida[*position].food;
	for(i=0;i<cantidad;i++){
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
 *  * * * * FUNCIONES PARA CONTROL BT* * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * *  * * * * * * * * * */

void USARTconfig()
{
	Chip_UART_Init(LPC_USART2);
	Chip_UART_SetBaud(LPC_USART2, 9600);  /* Set Baud rate */
	Chip_UART_SetupFIFOS(LPC_USART2, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0); /* Modify FCR (FIFO Control Register)*/
	Chip_UART_TXEnable(LPC_USART2); /* Enable UART Transmission */
	Chip_UART_IntEnable(LPC_USART2, UART_IER_RBRINT);
	/* Enable Interrupt for UART channel */
	/* Priority = 1 */
	NVIC_SetPriority(USART2_IRQn, 1);
	/* Enable Interrupt for UART channel */
	NVIC_EnableIRQ(USART2_IRQn);
	Chip_UART_Init(LPC_USART3);
	Chip_UART_SetBaud(LPC_USART3, 9600);  /* Set Baud rate */
	Chip_UART_SetupFIFOS(LPC_USART3, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0); /* Modify FCR (FIFO Control Register)*/
	Chip_UART_TXEnable(LPC_USART3); /* Enable UART Transmission */
	Chip_UART_IntEnable(LPC_USART3, UART_IER_RBRINT);
	/* Enable Interrupt for UART channel */
	/* Priority = 1 */
	NVIC_SetPriority(USART3_IRQn, 1);
	/* Enable Interrupt for UART channel */
	NVIC_EnableIRQ(USART3_IRQn);
}

/*
void parseBufferAlarm(char * buffer , food_t * ALARM)
{	char * aux;
	char* aux2;
	char * ChHora;
	uint8_t Hora;
	uint8_t Minutos;
	uint8_t comida;
	char * ChComida;
	/*formato recibido #HH:MM;NN&*/
	/*aux = strtok(buffer, ';' ); // separo la hora de la cantiad
	ChHora=aux;
		strcpy(Chhora,aux);
	aux=strtok(NULL,';');
	strcpy(ChComida,aux);

	ChComida[3]='\0';
	aux= strtok(ChHora, ':' );
	strcpy(aux2,aux);
	Hora=atoi(aux2);// me falta eliminar el # tengo que hacer una funcion que lo borre
	aux= strtok(NULL, ':' );
	strcpy(aux2,aux);
	aux2[3]='\0';
	Minutos=atoi(aux2);

	/*tengo q castear a uint8 y pasrlo al vector food*/

//}

/********* BLUETOOTH: **********/

#define OK_STR {(uint8_t)'O', (uint8_t)'K'}

bool compareStrings(uint8_t *str1, uint8_t *str2, int max){
	int i=0;
	for( ; i<max ; i++){
		if(str1[i]!=str2[i])
			return false;
	}
	return true;
}

int recibirBT(LPC_USART_T *pUART, uint8_t *data){

	int readBytes=0, bytesToRead=0;
	uint8_t *p8 = data;

	if ( (Chip_UART_ReadLineStatus(pUART) & UART_LSR_RDR) == 0 ){

		return -1;

	} else{

		bytesToRead = (int)Chip_UART_ReadByte(pUART);
	}

	while( readBytes < bytesToRead ){

		if ( (Chip_UART_ReadLineStatus(pUART) & UART_LSR_RDR) != 0 ){
			*p8 = Chip_UART_ReadByte(pUART);
			readBytes++;
			p8++;
		}
	}
	return readBytes;
}

int enviarBT(LPC_USART_T *pUART, uint8_t *data, int bytesToSend){

	int sentBytes = 0;
	uint8_t *p8 = data;

	while( sentBytes < bytesToSend ){

		if ( (Chip_UART_ReadLineStatus(pUART) & UART_LSR_THRE) != 0 ){
			Chip_UART_SendByte(pUART, *p8);
			sentBytes++;
			p8++;
		}
	}
	return sentBytes;
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

void UART2_IRQHandler()
{
	Chip_UART_IntDisable(LPC_USART3, UART_IER_RBRINT);
	USART2_IRQ_flag=true;
	//ReadTillEOL(LPC_USART2,recibido2,sizeof(recibido2));
	Chip_UART_IntDisable(LPC_USART2, UART_IER_RBRINT);
	Board_LED_Toggle(3);
}
void UART3_IRQHandler()
{
	Chip_UART_IntDisable(LPC_USART2, UART_IER_RBRINT);
	USART3_IRQ_flag=true;
	//ReadTillEOL(LPC_USART3,recibido0,sizeof(recibido0));
	Chip_UART_ReadBlocking(LPC_USART3, buffer, 8);
	Chip_UART_IntDisable(LPC_USART3, UART_IER_RBRINT);
	Chip_UART_ReadLineStatus(LPC_USART3);
	//Chip_UART_SetUpFIFO();
}


int main(void)
{
	/* Generic Initialization */
	fAlarmTimeMatched = 0;
	RTC_TIME_T FullTime;
	food_t VectorFood[MAX_POS];
	uint8_t pos=0;
	status_time aux;
	tick_ct=0;
	SystemCoreClockUpdate();
	Board_Init();
	int state =1;
	//uint8_t i;

	/* Initialize the SCT as PWM and set frequency */
	Chip_SCTPWM_Init(LPC_SCT);
	Chip_SCTPWM_SetRate(LPC_SCT, SCT_PWM_RATE);

	/* Setup Board specific output pin */
	Chip_SCU_PinMuxSet(0x4, 4, (SCU_MODE_INACT | SCU_MODE_FUNC1));	//CTOUT2=P4_4

	/* Use SCT0_OUT1 pin */
	Chip_SCTPWM_SetOutPin(LPC_SCT, 1, 2);

	/* Start with 0% duty cycle */
	Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_GetTicksPerCycle(LPC_SCT)*DUTY_CYCLE_full);
	Chip_SCTPWM_Start(LPC_SCT);
//	Chip_SCT_EnableEventInt(LPC_SCT, SCT_EVT_1);
	Chip_SCTPWM_Stop(LPC_SCT);

	/* Enable SysTick Timer */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);

	/*CONFIGURACION REAL CLOCK*/


	Chip_RTC_Init(LPC_RTC);

	/* Set current time for RTC 26:00:00PM, 2017-11-29 */
	FullTime.time[RTC_TIMETYPE_SECOND]  = 0;
	FullTime.time[RTC_TIMETYPE_MINUTE]  = 0;
	FullTime.time[RTC_TIMETYPE_HOUR]    = 16;
	FullTime.time[RTC_TIMETYPE_DAYOFMONTH]  = 29;
	FullTime.time[RTC_TIMETYPE_DAYOFWEEK]   = 3;
	FullTime.time[RTC_TIMETYPE_DAYOFYEAR]   = 332;
	FullTime.time[RTC_TIMETYPE_MONTH]   = 11;
	FullTime.time[RTC_TIMETYPE_YEAR]    = 2017;

	Chip_RTC_SetFullTime(LPC_RTC, &FullTime);

	/*inicializo el vector de comida con el horario del momento!*/
	VectorFoodInit(VectorFood,& FullTime);

	/* Set ALARM time */
	/*HARCODEO ALARMAS PARA PROBAR EN RAM*/ /*ESTO DESPUES DEBERIA RECIBIRLO POR BT */
	VectorFood[1].clock.time[RTC_TIMETYPE_SECOND]  = 15; // prueba d ealarma
	VectorFood[2].clock.time[RTC_TIMETYPE_SECOND]  = 30;
	VectorFood[0].clock.time[RTC_TIMETYPE_SECOND]  =45;

	VectorFood[1].food=1;
	VectorFood[2].food=2;
	VectorFood[0].food=3;

	/*ORDENO EL VECTOR DESORDENADO DE RAM*/
	aux=VectorAlarmSort(VectorFood);

	/*SETEO LA PRIMER ALARMA LUEGO DE ORDENAR EL VECTOR!*/
	Chip_RTC_SetFullAlarmTime(LPC_RTC, &VectorFood[0].clock);

	/* Enable matching for alarm for second, minute, hour fields only */
	Chip_RTC_AlarmIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC | RTC_AMR_CIIR_IMMIN | RTC_AMR_CIIR_IMHOUR, ENABLE);

	/* Clear interrupt pending */
	Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE | RTC_INT_ALARM);

	/* Enable RTC interrupt in NVIC */
	NVIC_EnableIRQ((IRQn_Type) RTC_IRQn);

	/* Enable RTC (starts increase the tick counter and second counter register) */
	Chip_RTC_Enable(LPC_RTC, ENABLE);

	USARTconfig();


	while (1) {


		__WFI();// ESPERA UNA ALARMA O UN EVENTO DE UART

	//toggle_servo ();

		if(fAlarmTimeMatched){
			fAlarmTimeMatched = false;

			Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
			showTime(&FullTime);
			showTime(&(VectorFood[pos].clock));
			DEBUGOUT("cantida: %.2d\r\n", VectorFood[pos].food);
			ServirComida(VectorFood,&pos);
		}

		/*if(USART3_IRQ_flag=true)
		{
			USART3_IRQ_flag=false;
			Chip_RTC_GetFullTime(LPC_RTC,&FullTime);
			showTime(&FullTime);

			DEBUGOUT("recibi en el buffer: %s \r\n", buffer);


		}*/
		/*if(state)
		{state=0;
		Chip_RTC_GetFullTime(LPC_RTC,&FullTime);
		showTime(&FullTime);
		}*/

	}
}
