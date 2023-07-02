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

BUILD_ARTIFACT_NAME := pico-os1
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)
BUILD_MAP = $(BUILD_ARTIFACT_NAME).map
BUILD_HEX = $(BUILD_ARTIFACT_NAME).hex
BUILD_SIZE = $(BUILD_ARTIFACT_NAME).siz
export picotarget = $(realpath $(BUILD_HEX))

DEPS := $(OBJS:.o=.d)
CFLAGS := -std=c99 -mcpu=cortex-m0plus -mthumb -O0 -ffreestanding -g3 -MMD -MP -I"../include"
LDFLAGS := -lc -T "../linker/pico_memmap.ld" -Xlinker --gc-sections -Wl,-Map,$(BUILD_MAP)

# All Target
all: main-build

# Main-build Target
main-build: $(BUILD_ARTIFACT) secondary-outputs

# Tool invocations
$(BUILD_ARTIFACT): $(OBJS) $(USER_OBJS) Makefile objects.mk
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Arm Cross C Linker'
	arm-none-eabi-gcc $(CFLAGS) -static -nostartfiles -g3 $(LDFLAGS) -o $(BUILD_ARTIFACT) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(BUILD_HEX): $(BUILD_ARTIFACT) Makefile objects.mk
	@echo 'Invoking: GNU Arm Cross Create Flash Image'
	arm-none-eabi-objcopy -O ihex $(BUILD_ARTIFACT) $(BUILD_HEX)
	@echo 'Finished building: $@'
	@echo ' '

$(BUILD_SIZE): $(BUILD_ARTIFACT) Makefile objects.mk
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
	cd  ~/repo/sokoide/rp2040js && npm start

debug: $(BUILD_HEX)
	cd  ~/repo/sokoide/rp2040js && node --nolazy -r ts-node/register/transpile-only --inspect demo/emulator-run.ts

reset: $(BUILD_HEX)
	ps -ef|grep node | grep emulator-run |grep -v grep |awk '{print("kill -HUP "$$2)}'| sh

.PHONY: all clean dependents main-build

-include ../makefile.targets
