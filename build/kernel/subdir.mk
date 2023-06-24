# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../kernel/syslib.c

OBJS += \
./kernel/syslib.o

C_DEPS += \
./kernel/syslib.d


# Each subdirectory must supply rules for building sources it contributes
kernel/%.o: ../kernel/%.c kernel/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -ffreestanding -g3 -I"../include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


