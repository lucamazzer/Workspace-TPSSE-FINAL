#ifndef FUNCIONES_H_
#define FUNCIONES_H_


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

static volatile bool fIntervalReached;
static volatile bool fAlarmTimeMatched;
static volatile bool On0, On1;
typedef struct{

	RTC_TIME_T clock;
	uint8_t food;
	bool init;

}food_t;


#define BOARD_NXP_LPCXPRESSO_4337

#define SCT_PWM_RATE   50        /* PWM frequency 50 Hz */
/* Systick timer tick rate, to change duty cycle */
#define TICKRATE_HZ     100        /* 1 ms Tick rate */

#define DUTY_CYCLE 90/100	//Duty Cycle configurado 180 grados
#define DUTY_CYCLE_full 98/100	//Duty Cycle configurado 0 grados

volatile bool ServoFlag=false;
static uint32_t tick_ct = 0;
static volatile bool fAlarmTimeMatched;


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
#define MAX_POS 3
/*****************************************************************************
 * Private functions
 ****************************************************************************/



static void showTime(RTC_TIME_T *pTime);

/*****************************************************************************
 * Public functions
 ****************************************************************************/

void VectorFoodInit(food_t * VectorFood, RTC_TIME_T FullTime);
void set_new_alarm(RTC_TIME_T * AlarmVector, uint8_t pos);
void swap(RTC_TIME_T * AlarmVector, uint8_t pos1,uint8_t pos2);
void VAlarm_ordenar(RTC_TIME_T * AlarmVector);
int RTC(void); /* ejemplo de uso de rtc*/

#endif /* FUNCIONES_H_ */
