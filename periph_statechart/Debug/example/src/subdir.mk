################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/Rom.c \
../example/src/TimerTicks.c \
../example/src/cr_startup_lpc43xx.c \
../example/src/statechart.c \
../example/src/sysinit.c 

OBJS += \
./example/src/Rom.o \
./example/src/TimerTicks.o \
./example/src/cr_startup_lpc43xx.o \
./example/src/statechart.o \
./example/src/sysinit.o 

C_DEPS += \
./example/src/Rom.d \
./example/src/TimerTicks.d \
./example/src/cr_startup_lpc43xx.d \
./example/src/statechart.d \
./example/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__MULTICORE_NONE -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -D__REDLIB__ -DCORE_M4 -I"C:\Users\luca10\Desktop\EMbebidos\Workspace-TPSSE-FINAL\lpc_chip_43xx\inc" -I"C:\Users\luca10\Desktop\EMbebidos\Workspace-TPSSE-FINAL\lpc_board_nxp_lpcxpresso_4337\inc" -I"C:\Users\luca10\Desktop\EMbebidos\Workspace-TPSSE-FINAL\periph_statechart\example\src\src-gen" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


