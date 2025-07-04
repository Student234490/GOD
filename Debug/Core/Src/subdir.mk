################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/fixp.c \
../Core/Src/gps.c \
../Core/Src/i2c.c \
../Core/Src/igrf16.c \
../Core/Src/lcd.c \
../Core/Src/lsm9ds1.c \
../Core/Src/luts.c \
../Core/Src/main.c \
../Core/Src/stm32g4xx_hal_msp.c \
../Core/Src/stm32g4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g4xx.c \
../Core/Src/triad.c \
../Core/Src/vector.c 

OBJS += \
./Core/Src/fixp.o \
./Core/Src/gps.o \
./Core/Src/i2c.o \
./Core/Src/igrf16.o \
./Core/Src/lcd.o \
./Core/Src/lsm9ds1.o \
./Core/Src/luts.o \
./Core/Src/main.o \
./Core/Src/stm32g4xx_hal_msp.o \
./Core/Src/stm32g4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g4xx.o \
./Core/Src/triad.o \
./Core/Src/vector.o 

C_DEPS += \
./Core/Src/fixp.d \
./Core/Src/gps.d \
./Core/Src/i2c.d \
./Core/Src/igrf16.d \
./Core/Src/lcd.d \
./Core/Src/lsm9ds1.d \
./Core/Src/luts.d \
./Core/Src/main.d \
./Core/Src/stm32g4xx_hal_msp.d \
./Core/Src/stm32g4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g4xx.d \
./Core/Src/triad.d \
./Core/Src/vector.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/fixp.d ./Core/Src/fixp.o ./Core/Src/gps.d ./Core/Src/gps.o ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/igrf16.d ./Core/Src/igrf16.o ./Core/Src/lcd.d ./Core/Src/lcd.o ./Core/Src/lsm9ds1.d ./Core/Src/lsm9ds1.o ./Core/Src/luts.d ./Core/Src/luts.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/stm32g4xx_hal_msp.d ./Core/Src/stm32g4xx_hal_msp.o ./Core/Src/stm32g4xx_it.d ./Core/Src/stm32g4xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32g4xx.d ./Core/Src/system_stm32g4xx.o ./Core/Src/triad.d ./Core/Src/triad.o ./Core/Src/vector.d ./Core/Src/vector.o

.PHONY: clean-Core-2f-Src

