################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../boot/boot2.c \
../boot/reset_handler.c \
../boot/vector_table.c


OBJS += \
./boot/boot2.o \
./boot/reset_handler.o \
./boot/vector_table.o


# Each subdirectory must supply rules for building sources it contributes
boot/%.o: ../boot/%.c boot/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -ffreestanding -g3 -I"../include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
