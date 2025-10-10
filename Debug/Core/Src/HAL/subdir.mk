################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/HAL/bmp280_program.c \
../Core/Src/HAL/esp01_program.c \
../Core/Src/HAL/mq6_program.c 

OBJS += \
./Core/Src/HAL/bmp280_program.o \
./Core/Src/HAL/esp01_program.o \
./Core/Src/HAL/mq6_program.o 

C_DEPS += \
./Core/Src/HAL/bmp280_program.d \
./Core/Src/HAL/esp01_program.d \
./Core/Src/HAL/mq6_program.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/HAL/%.o Core/Src/HAL/%.su Core/Src/HAL/%.cyclo: ../Core/Src/HAL/%.c Core/Src/HAL/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Core/Inc/SL -I../Core/Inc/SL/MQTT -I../Core/Inc/MCAL -I../Core/Inc/MCAL/ADC -I../Core/Inc/MCAL/GPIO -I../Core/Inc/MCAL/NVIC -I../Core/Inc/MCAL/RCC -I../Core/Inc/MCAL/STK -I../Core/Inc/MCAL/USART -I../Core/Inc/HAL -I../Core/Inc/HAL/ESP -I../Core/Inc/HAL/MQ6 -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc/MCAL/I2C -I../Core/Inc/HAL/BMP280 -I../Core/Inc/MCAL/TIM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-HAL

clean-Core-2f-Src-2f-HAL:
	-$(RM) ./Core/Src/HAL/bmp280_program.cyclo ./Core/Src/HAL/bmp280_program.d ./Core/Src/HAL/bmp280_program.o ./Core/Src/HAL/bmp280_program.su ./Core/Src/HAL/esp01_program.cyclo ./Core/Src/HAL/esp01_program.d ./Core/Src/HAL/esp01_program.o ./Core/Src/HAL/esp01_program.su ./Core/Src/HAL/mq6_program.cyclo ./Core/Src/HAL/mq6_program.d ./Core/Src/HAL/mq6_program.o ./Core/Src/HAL/mq6_program.su

.PHONY: clean-Core-2f-Src-2f-HAL

