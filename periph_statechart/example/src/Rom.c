#include "board.h"

#define BOARD_NXP_LPCXPRESSO_4337


typedef struct{

	RTC_TIME_T clock;
	uint8_t food;
	bool init;
	bool nextday;

}food_t;

/* PARA ESCRIBIR EN ROM*/
#define START_ADDR_LAST_SECTOR                 0x1A070000 // NUMERO DEMEMORIA DEL SECTOR
#define SECTOR_SIZE                            65536 // 64KB TAMAÑO DEL SECTOR
#define IAP_LAST_SECTOR                        14 // NUMERO DE SECTOR ( SON 2 BANCOS CON 14 SECTORES)
#define IAP_NUM_BYTES_TO_WRITE                 3*(sizeof(food_t))// tamañó de memoria que voya escribir 3 pedazos de estructura
/* Number elements in array */
#define ARRAY_ELEMENTS          (IAP_NUM_BYTES_TO_WRITE / sizeof(uint32_t)) // esto no seria necesario

/* Data array to write to flash */
static uint32_t src_iap_array_data[ARRAY_ELEMENTS];
static food_t  Array_rom[3]; // array paraescribir en rom


/* carga los datos de rom en un vector en RAM */


	/* Initialize the array data to be written to FLASH */
		/*for (i = 0; i < ARRAY_ELEMENTS; i++) {
			src_iap_array_data[i] = 0x11223340 + i;
		}*/

		/* Read Part Identification Number*/
		/*part_id = Chip_IAP_ReadPID();
		DEBUGOUT("Part ID is: %x\r\n", part_id);*/

		/* Disable interrupt mode so it doesn't fire during FLASH updates */
int Rom(void){

	uint8_t ret_code;

		__disable_irq();

		/* IAP Flash programming */
		/* Prepare to write/erase the last sector */
		ret_code = Chip_IAP_PreSectorForReadWrite(IAP_LAST_SECTOR, IAP_LAST_SECTOR, IAP_FLASH_BANK_A);

		/* Error checking */
		if (ret_code != IAP_CMD_SUCCESS) {
		//	DEBUGOUT("Command failed to execute, return code is: %x\r\n", ret_code);
		}

		/* Erase the last sector */
		ret_code = Chip_IAP_EraseSector(IAP_LAST_SECTOR, IAP_LAST_SECTOR, IAP_FLASH_BANK_A);

		/* Error checking */
		if (ret_code != IAP_CMD_SUCCESS) {
			//DEBUGOUT("Command failed to execute, return code is: %x\r\n", ret_code);
		}

		/* Prepare to write/erase the last sector */
		ret_code = Chip_IAP_PreSectorForReadWrite(IAP_LAST_SECTOR, IAP_LAST_SECTOR, IAP_FLASH_BANK_A);

		/* Error checking */
		if (ret_code != IAP_CMD_SUCCESS) {
			//DEBUGOUT("Command failed to execute, return code is: %x\r\n", ret_code);
		}

		/* Write to the last sector */
		ret_code = Chip_IAP_CopyRamToFlash(START_ADDR_LAST_SECTOR, Array_rom, IAP_NUM_BYTES_TO_WRITE);

		/* Error checking */
		if (ret_code != IAP_CMD_SUCCESS) {
		//	DEBUGOUT("Command failed to execute, return code is: %x\r\n", ret_code);
		}

		/* Re-enable interrupt mode */
		__enable_irq();

		/* Start the signature generator for the last sector */
		Chip_FMC_ComputeSignatureBlocks(0, START_ADDR_LAST_SECTOR, (SECTOR_SIZE / 16));

		/* Check for signature geenration completion */
		while (Chip_FMC_IsSignatureBusy(0)) {}

		/* Get the generated FLASH signature value */
		//DEBUGOUT("Generated signature for the last sector is: %x \r\n", Chip_FMC_GetSignature(0, 0));

}
