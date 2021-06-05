################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Device/Driver/Src/r_dmac/r_dmac_api.c 

OBJS += \
./Device/Driver/Src/r_dmac/r_dmac_api.o 

C_DEPS += \
./Device/Driver/Src/r_dmac/r_dmac_api.d 


# Each subdirectory must supply rules for building sources it contributes
Device/Driver/Src/r_dmac/%.o: ../Device/Driver/Src/r_dmac/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc --specs=nosys.specs -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device\Driver\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device\CMSIS_Driver\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device\Config" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\CMSIS\Core\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\CMSIS\Driver\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\CMSIS\DSP_Lib\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\src\tinycrypt" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\src\src" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\src\key" -IE:/SHC/RE01/examples/re01_256kb_ek_bootloader/generate -IE:/SHC/RE01/examples/re01_256kb_ek_bootloader/src -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


