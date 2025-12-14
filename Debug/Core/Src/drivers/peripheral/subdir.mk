################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/drivers/peripheral/adc.c \
../Core/Src/drivers/peripheral/gpio.c \
../Core/Src/drivers/peripheral/i2c.c \
../Core/Src/drivers/peripheral/iwdg.c \
../Core/Src/drivers/peripheral/nvic.c \
../Core/Src/drivers/peripheral/rcc.c \
../Core/Src/drivers/peripheral/stk.c \
../Core/Src/drivers/peripheral/tim.c \
../Core/Src/drivers/peripheral/usart.c 

OBJS += \
./Core/Src/drivers/peripheral/adc.o \
./Core/Src/drivers/peripheral/gpio.o \
./Core/Src/drivers/peripheral/i2c.o \
./Core/Src/drivers/peripheral/iwdg.o \
./Core/Src/drivers/peripheral/nvic.o \
./Core/Src/drivers/peripheral/rcc.o \
./Core/Src/drivers/peripheral/stk.o \
./Core/Src/drivers/peripheral/tim.o \
./Core/Src/drivers/peripheral/usart.o 

C_DEPS += \
./Core/Src/drivers/peripheral/adc.d \
./Core/Src/drivers/peripheral/gpio.d \
./Core/Src/drivers/peripheral/i2c.d \
./Core/Src/drivers/peripheral/iwdg.d \
./Core/Src/drivers/peripheral/nvic.d \
./Core/Src/drivers/peripheral/rcc.d \
./Core/Src/drivers/peripheral/stk.d \
./Core/Src/drivers/peripheral/tim.d \
./Core/Src/drivers/peripheral/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/drivers/peripheral/%.o Core/Src/drivers/peripheral/%.su Core/Src/drivers/peripheral/%.cyclo: ../Core/Src/drivers/peripheral/%.c Core/Src/drivers/peripheral/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Core/Inc/SL -I../Core/Inc/SL/MQTT -I../Core/Inc/MCAL -I../Core/Inc/MCAL/ADC -I../Core/Inc/MCAL/GPIO -I../Core/Inc/MCAL/NVIC -I../Core/Inc/MCAL/RCC -I../Core/Inc/MCAL/STK -I../Core/Inc/MCAL/USART -I../Core/Inc/HAL -I../Core/Inc/HAL/ESP -I../Core/Inc/HAL/MQ6 -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc/MCAL/I2C -I../Core/Inc/HAL/BMP280 -I../Core/Inc/MCAL/TIM -I../Core/Inc/MCAL/IWDG -I../Core/Inc/drivers/peripheral -I../Core/Inc/drivers/device -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-drivers-2f-peripheral

clean-Core-2f-Src-2f-drivers-2f-peripheral:
	-$(RM) ./Core/Src/drivers/peripheral/adc.cyclo ./Core/Src/drivers/peripheral/adc.d ./Core/Src/drivers/peripheral/adc.o ./Core/Src/drivers/peripheral/adc.su ./Core/Src/drivers/peripheral/gpio.cyclo ./Core/Src/drivers/peripheral/gpio.d ./Core/Src/drivers/peripheral/gpio.o ./Core/Src/drivers/peripheral/gpio.su ./Core/Src/drivers/peripheral/i2c.cyclo ./Core/Src/drivers/peripheral/i2c.d ./Core/Src/drivers/peripheral/i2c.o ./Core/Src/drivers/peripheral/i2c.su ./Core/Src/drivers/peripheral/iwdg.cyclo ./Core/Src/drivers/peripheral/iwdg.d ./Core/Src/drivers/peripheral/iwdg.o ./Core/Src/drivers/peripheral/iwdg.su ./Core/Src/drivers/peripheral/nvic.cyclo ./Core/Src/drivers/peripheral/nvic.d ./Core/Src/drivers/peripheral/nvic.o ./Core/Src/drivers/peripheral/nvic.su ./Core/Src/drivers/peripheral/rcc.cyclo ./Core/Src/drivers/peripheral/rcc.d ./Core/Src/drivers/peripheral/rcc.o ./Core/Src/drivers/peripheral/rcc.su ./Core/Src/drivers/peripheral/stk.cyclo ./Core/Src/drivers/peripheral/stk.d ./Core/Src/drivers/peripheral/stk.o ./Core/Src/drivers/peripheral/stk.su ./Core/Src/drivers/peripheral/tim.cyclo ./Core/Src/drivers/peripheral/tim.d ./Core/Src/drivers/peripheral/tim.o ./Core/Src/drivers/peripheral/tim.su ./Core/Src/drivers/peripheral/usart.cyclo ./Core/Src/drivers/peripheral/usart.d ./Core/Src/drivers/peripheral/usart.o ./Core/Src/drivers/peripheral/usart.su

.PHONY: clean-Core-2f-Src-2f-drivers-2f-peripheral

