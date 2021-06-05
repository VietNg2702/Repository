################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#-include ../makefile.init

#-include makefile.init

RM := rm -rf *.lst *.lis *.lpp *.map

# All of the sources participating in the build are defined here
-include sources.mk
-include src/tinycrypt/subdir.mk
-include src/src/subdir.mk
-include src/subdir.mk
-include script/subdir.mk
-include Device/Driver/Src/r_system/subdir.mk
-include Device/Driver/Src/r_lpm/subdir.mk
-include Device/Driver/Src/r_fwup/src/target/re01_256kb/subdir.mk
-include Device/Driver/Src/r_fwup/src/subdir.mk
-include Device/Driver/Src/r_flash/subdir.mk
-include Device/Driver/Src/r_dtc/subdir.mk
-include Device/Driver/Src/r_dmac/subdir.mk
-include Device/CMSIS_Driver/Src/subdir.mk
-include Device/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(SRC_DEPS)),)
-include $(SRC_DEPS)
endif
ifneq ($(strip $(JMP_DEPS)),)
-include $(JMP_DEPS)
endif
ifneq ($(strip $(PP_DEPS)),)
-include $(PP_DEPS)
endif
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(FSY_DEPS)),)
-include $(FSY_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(P_DEPS)),)
-include $(P_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

#-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
re01_256kb_ek_bootloader.hex \

SECONDARY_SIZE += \
re01_256kb_ek_bootloader.siz \


# All Target
all: re01_256kb_ek_bootloader.elf re01_256kb_ek_bootloader.hex re01_256kb_ek_bootloader.siz

# Tool invocations
re01_256kb_ek_bootloader.elf: $(OBJS) $(USER_OBJS) $(LINKER_SCRIPT)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM Cross C Linker'
	arm-none-eabi-g++ --specs=nosys.specs -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -T"../script/RE01_256KB_bootloader_64k.ld" -Xlinker --gc-sections -Wl,-Map,"re01_256kb_ek_bootloader.map" -o "re01_256kb_ek_bootloader.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	echo @"re01_256kb_ek_bootloader.elf.in"
	@echo 'Finished building target: $@'
	@echo ' '

re01_256kb_ek_bootloader.hex: re01_256kb_ek_bootloader.elf
	@echo 'Invoking: GNU ARM Cross Create Flash Image'
	arm-none-eabi-objcopy -O ihex "re01_256kb_ek_bootloader.elf"  "re01_256kb_ek_bootloader.hex"
	@echo 'Finished building: $@'
	@echo ' '

re01_256kb_ek_bootloader.siz: re01_256kb_ek_bootloader.elf
	@echo 'Invoking: GNU ARM Cross Print Size'
	arm-none-eabi-size --format=berkeley "re01_256kb_ek_bootloader.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
ifneq ($(strip $(SRC_DEPS)),)
	-$(file > clean_SRC_DEPS.tmp, $(SRC_DEPS))
	-@ xargs -s 6000 -t -a clean_SRC_DEPS.tmp $(RM)
	-@$(RM) clean_SRC_DEPS.tmp
endif
ifneq ($(strip $(JMP_DEPS)),)
	-$(file > clean_JMP_DEPS.tmp, $(JMP_DEPS))
	-@ xargs -s 6000 -t -a clean_JMP_DEPS.tmp $(RM)
	-@$(RM) clean_JMP_DEPS.tmp
endif
ifneq ($(strip $(PP_DEPS)),)
	-$(file > clean_PP_DEPS.tmp, $(PP_DEPS))
	-@ xargs -s 6000 -t -a clean_PP_DEPS.tmp $(RM)
	-@$(RM) clean_PP_DEPS.tmp
endif
ifneq ($(strip $(SX_DEPS)),)
	-$(file > clean_SX_DEPS.tmp, $(SX_DEPS))
	-@ xargs -s 6000 -t -a clean_SX_DEPS.tmp $(RM)
	-@$(RM) clean_SX_DEPS.tmp
endif
ifneq ($(strip $(FSY_DEPS)),)
	-$(file > clean_FSY_DEPS.tmp, $(FSY_DEPS))
	-@ xargs -s 6000 -t -a clean_FSY_DEPS.tmp $(RM)
	-@$(RM) clean_FSY_DEPS.tmp
endif
ifneq ($(strip $(OBJS)),)
	-$(file > clean_OBJS.tmp, $(OBJS))
	-@ xargs -s 6000 -t -a clean_OBJS.tmp $(RM)
	-@$(RM) clean_OBJS.tmp
endif
ifneq ($(strip $(SECONDARY_FLASH)),)
	-$(file > clean_SECONDARY_FLASH.tmp, $(SECONDARY_FLASH))
	-@ xargs -s 6000 -t -a clean_SECONDARY_FLASH.tmp $(RM)
	-@$(RM) clean_SECONDARY_FLASH.tmp
endif
ifneq ($(strip $(SECONDARY_SIZE)),)
	-$(file > clean_SECONDARY_SIZE.tmp, $(SECONDARY_SIZE))
	-@ xargs -s 6000 -t -a clean_SECONDARY_SIZE.tmp $(RM)
	-@$(RM) clean_SECONDARY_SIZE.tmp
endif
ifneq ($(strip $(ASM_DEPS)),)
	-$(file > clean_ASM_DEPS.tmp, $(ASM_DEPS))
	-@ xargs -s 6000 -t -a clean_ASM_DEPS.tmp $(RM)
	-@$(RM) clean_ASM_DEPS.tmp
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
	-$(file > clean_S_UPPER_DEPS.tmp, $(S_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_S_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_S_UPPER_DEPS.tmp
endif
ifneq ($(strip $(P_DEPS)),)
	-$(file > clean_P_DEPS.tmp, $(P_DEPS))
	-@ xargs -s 6000 -t -a clean_P_DEPS.tmp $(RM)
	-@$(RM) clean_P_DEPS.tmp
endif
ifneq ($(strip $(C_DEPS)),)
	-$(file > clean_C_DEPS.tmp, $(C_DEPS))
	-@ xargs -s 6000 -t -a clean_C_DEPS.tmp $(RM)
	-@$(RM) clean_C_DEPS.tmp
endif
	-$(RM)  re01_256kb_ek_bootloader.elf
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
