.DEFAULT_GOAL := all
RM := rm -rf


# All of the sources participating in the build are defined here
-include sources.mk
-include boot/subdir.mk
-include application/subdir.mk
-include device/subdir.mk
-include kernel/subdir.mk
-include subdir.mk
-include objects.mk


CROSS_INC := ~/APFSCS/cross/rpi/arm-unknown-eabi/arm-unknown-eabi/include
CROSS_LIB := ~/APFSCS/cross/rpi/arm-unknown-eabi/arm-unknown-eabi/lib

BUILD_ARTIFACT_NAME := pico-os1
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)
BUILD_MAP = $(BUILD_ARTIFACT_NAME).map
BUILD_HEX = $(BUILD_ARTIFACT_NAME).hex
BUILD_SIZE = $(BUILD_ARTIFACT_NAME).siz

DEPS := $(OBJS:.o=.d)

# All Target
all: main-build

# Main-build Target
main-build: $(BUILD_ARTIFACT) secondary-outputs

# Tool invocations
$(BUILD_ARTIFACT): $(OBJS) $(USER_OBJS) makefile objects.mk
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Arm Cross C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -ffreestanding -g3 -L$(CROSS_LIB) -lc  -T "../linker/pico_memmap.ld" -Xlinker --gc-sections -Wl,-Map,$(BUILD_MAP) -o $(BUILD_ARTIFACT) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(BUILD_HEX): $(BUILD_ARTIFACT) makefile objects.mk
	@echo 'Invoking: GNU Arm Cross Create Flash Image'
	arm-none-eabi-objcopy -O ihex $(BUILD_ARTIFACT) $(BUILD_HEX)
	@echo 'Finished building: $@'
	@echo ' '

$(BUILD_SIZE): $(BUILD_ARTIFACT) makefile objects.mk
	@echo 'Invoking: GNU Arm Cross Print Size'
	arm-none-eabi-size --format=berkeley $(BUILD_ARTIFACT)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS) $(BUILD_HEX) $(BUILD_SIZE) $(BUILD_ARTIFACT) $(BUILD_MAP) $(DEPS)
	-@echo ' '

secondary-outputs: $(BUILD_HEX) $(BUILD_SIZE)

run: $(BUILD_HEX)
	cp $(BUILD_HEX) ~/repo/oss/rp2040js/hello_uart.hex
	cd  ~/repo/oss/rp2040js && npm start

.PHONY: all clean dependents main-build

-include ../makefile.targets
