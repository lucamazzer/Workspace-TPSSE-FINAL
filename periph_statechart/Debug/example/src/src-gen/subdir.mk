################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/src-gen/Prefix.c 

OBJS += \
./example/src/src-gen/Prefix.o 

C_DEPS += \
./example/src/src-gen/Prefix.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/src-gen/%.o: ../example/src/src-gen/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__MULTICORE_NONE -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -D__REDLIB__ -DCORE_M4 -I"C:\Users\luca10\Desktop\EMbebidos\Workspace-tpfinal\lpc_chip_43xx\inc" -I"C:\Users\luca10\Desktop\EMbebidos\Workspace-tpfinal\lpc_board_nxp_lpcxpresso_4337\inc" -I"C:\Users\luca10\Desktop\EMbebidos\Workspace-tpfinal\periph_statechart\example\src\src-gen" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

