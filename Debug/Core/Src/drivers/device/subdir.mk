################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/drivers/device/bmp280.c \
../Core/Src/drivers/device/esp01.c \
../Core/Src/drivers/device/mq6.c \
../Core/Src/drivers/device/mqtt.c \
../Core/Src/drivers/device/tsl2561.c 

OBJS += \
./Core/Src/drivers/device/bmp280.o \
./Core/Src/drivers/device/esp01.o \
./Core/Src/drivers/device/mq6.o \
./Core/Src/drivers/device/mqtt.o \
./Core/Src/drivers/device/tsl2561.o 

C_DEPS += \
./Core/Src/drivers/device/bmp280.d \
./Core/Src/drivers/device/esp01.d \
./Core/Src/drivers/device/mq6.d \
./Core/Src/drivers/device/mqtt.d \
./Core/Src/drivers/device/tsl2561.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/drivers/device/%.o Core/Src/drivers/device/%.su Core/Src/drivers/device/%.cyclo: ../Core/Src/drivers/device/%.c Core/Src/drivers/device/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Core/Inc/SL -I../Core/Inc/SL/MQTT -I../Core/Inc/MCAL -I../Core/Inc/MCAL/ADC -I../Core/Inc/MCAL/GPIO -I../Core/Inc/MCAL/NVIC -I../Core/Inc/MCAL/RCC -I../Core/Inc/MCAL/STK -I../Core/Inc/MCAL/USART -I../Core/Inc/HAL -I../Core/Inc/HAL/ESP -I../Core/Inc/HAL/MQ6 -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc/MCAL/I2C -I../Core/Inc/HAL/BMP280 -I../Core/Inc/MCAL/TIM -I../Core/Inc/MCAL/IWDG -I../Core/Inc/drivers/peripheral -I../Core/Inc/drivers/device -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-drivers-2f-device

clean-Core-2f-Src-2f-drivers-2f-device:
	-$(RM) ./Core/Src/drivers/device/bmp280.cyclo ./Core/Src/drivers/device/bmp280.d ./Core/Src/drivers/device/bmp280.o ./Core/Src/drivers/device/bmp280.su ./Core/Src/drivers/device/esp01.cyclo ./Core/Src/drivers/device/esp01.d ./Core/Src/drivers/device/esp01.o ./Core/Src/drivers/device/esp01.su ./Core/Src/drivers/device/mq6.cyclo ./Core/Src/drivers/device/mq6.d ./Core/Src/drivers/device/mq6.o ./Core/Src/drivers/device/mq6.su ./Core/Src/drivers/device/mqtt.cyclo ./Core/Src/drivers/device/mqtt.d ./Core/Src/drivers/device/mqtt.o ./Core/Src/drivers/device/mqtt.su ./Core/Src/drivers/device/tsl2561.cyclo ./Core/Src/drivers/device/tsl2561.d ./Core/Src/drivers/device/tsl2561.o ./Core/Src/drivers/device/tsl2561.su

.PHONY: clean-Core-2f-Src-2f-drivers-2f-device

