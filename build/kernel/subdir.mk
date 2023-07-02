# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../kernel/context.c \
../kernel/task_manager.c \
../kernel/systimer.c \
../kernel/event.c \
../kernel/syscalls.c \
../kernel/uart.c \

ASM_SRCS += \
../kernel/dispatch.S

OBJS += \
./kernel/dispatch.o \
./kernel/context.o \
./kernel/task_manager.o \
./kernel/systimer.o \
./kernel/event.o \
./kernel/syscalls.o \
./kernel/uart.o \



# Each subdirectory must supply rules for building sources it contributes
kernel/%.o: ../kernel/%.c kernel/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -ffreestanding -g3 -I"../include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel/%.o: ../kernel/%.S kernel/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -ffreestanding -g3 -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
