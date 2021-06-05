################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Device/CMSIS_Driver/Src/r_i2c_cmsis_api.c \
../Device/CMSIS_Driver/Src/r_spi_cmsis_api.c \
../Device/CMSIS_Driver/Src/r_usart_cmsis_api.c 

OBJS += \
./Device/CMSIS_Driver/Src/r_i2c_cmsis_api.o \
./Device/CMSIS_Driver/Src/r_spi_cmsis_api.o \
./Device/CMSIS_Driver/Src/r_usart_cmsis_api.o 

C_DEPS += \
./Device/CMSIS_Driver/Src/r_i2c_cmsis_api.d \
./Device/CMSIS_Driver/Src/r_spi_cmsis_api.d \
./Device/CMSIS_Driver/Src/r_usart_cmsis_api.d 


# Each subdirectory must supply rules for building sources it contributes
Device/CMSIS_Driver/Src/%.o: ../Device/CMSIS_Driver/Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc --specs=nosys.specs -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device\Driver\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device\CMSIS_Driver\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\Device\Config" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\CMSIS\Core\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\CMSIS\Driver\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\CMSIS\DSP_Lib\Include" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\src\tinycrypt" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\src\src" -I"E:\SHC\RE01\examples\re01_256kb_ek_bootloader\src\key" -IE:/SHC/RE01/examples/re01_256kb_ek_bootloader/generate -IE:/SHC/RE01/examples/re01_256kb_ek_bootloader/src -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


