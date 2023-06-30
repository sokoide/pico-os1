################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../application/usermain.c \
../application/lcd.c \
../application/ssd1306.c \
../application/syscalls.c


OBJS += \
./application/usermain.o \
./application/lcd.o \
./application/ssd1306.o \
./application/syscalls.o


# Each subdirectory must supply rules for building sources it contributes
application/%.o: ../application/%.c application/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	#arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -ffreestanding -g3 -I"../include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	# ~/APFSCS/cross/rpi/arm-unknown-eabi/arm-unknown-eabi/lib
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -ffreestanding -g3 -I"../include" -I"~/APFSCS/cross/rpi/arm-unknown-eabi/arm-unknown-eabi/include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
