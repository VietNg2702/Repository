################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Device/pin.c \
../Device/startup_RE01_256KB.c \
../Device/system_RE01_256KB.c 

OBJS += \
./Device/pin.o \
./Device/startup_RE01_256KB.o \
./Device/system_RE01_256KB.o 

C_DEPS += \
./Device/pin.d \
./Device/startup_RE01_256KB.d \
./Device/system_RE01_256KB.d 


# Each subdirectory must supply rules for building sources it contributes
Device/%.o: ../Device/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc --specs=nosys.specs -mcpu=cortex-m0plus -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device\Driver\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device\CMSIS_Driver\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device\Config" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\CMSIS\Core\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\CMSIS\Driver\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\CMSIS\DSP_Lib\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\src\tinycrypt" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\src\src" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\src\key" -IE:/SHC/RE01/examples/re01_256kb_ek_bootloader/generate -IE:/SHC/RE01/examples/re01_256kb_ek_bootloader/src -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


