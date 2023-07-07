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
EMULATOR_PATH = $(HOME)/repo/sokoide/rp2040js
export picotarget = $(realpath $(BUILD_HEX))
VOL := KOIF12
IMG := koidos.fat

DEPS := $(OBJS:.o=.d)
CFLAGS := -std=c99 -mcpu=cortex-m0plus -mthumb -O0 -ffreestanding -g3 -MMD -MP -I"../include"
LDFLAGS := -lc -T "../linker/pico_memmap.ld" -Xlinker --gc-sections -Wl,-Map,$(BUILD_MAP) -Wl,--section-start=.fs=0x18000000

# All Target
all: main-build

# Main-build Target
main-build: $(BUILD_ARTIFACT) secondary-outputs

# Tool invocations
$(BUILD_ARTIFACT): $(OBJS) $(USER_OBJS) Makefile objects.mk koidos.fat
	@echo 'Converting a fs file'
	arm-none-eabi-objcopy -I binary -O elf32-littlearm -B armv6s-m --rename-section .data=.fs_section koidos.fat koidos.o
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

fat12:
	rm $(IMG)
	mformat -f 720 -v $(VOL) -C -i $(IMG) ::

	echo "hello world" > hello.txt
	echo "konnnichiwa" > helloj.txt
	echo "I am hoge." > hoge.txt
	echo "You are page." > page.txt

	mcopy -i $(IMG) hello.txt helloj.txt hoge.txt page.txt ::
	mdir -i $(IMG)
	rm hello*.txt hoge.txt page.txt

run: $(BUILD_HEX)
	cd  $(EMULATOR_PATH) && npm start

debug: $(BUILD_HEX)
	cd  $(EMULATOR_PATH) && node --nolazy -r ts-node/register/transpile-only --inspect demo/emulator-run.ts

reset: $(BUILD_HEX)
	ps -ef|grep node | grep emulator-run |grep -v grep |awk '{print("kill -HUP "$$2)}'| sh

.PHONY: all clean dependents main-build fat12

-include ../makefile.targets
