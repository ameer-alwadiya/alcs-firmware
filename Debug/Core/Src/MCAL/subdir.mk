################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/MCAL/adc_program.c \
../Core/Src/MCAL/gpio_program.c \
../Core/Src/MCAL/i2c_program.c \
../Core/Src/MCAL/nvic_program.c \
../Core/Src/MCAL/rcc_program.c \
../Core/Src/MCAL/stk_program.c \
../Core/Src/MCAL/tim.c \
../Core/Src/MCAL/usart_program.c 

OBJS += \
./Core/Src/MCAL/adc_program.o \
./Core/Src/MCAL/gpio_program.o \
./Core/Src/MCAL/i2c_program.o \
./Core/Src/MCAL/nvic_program.o \
./Core/Src/MCAL/rcc_program.o \
./Core/Src/MCAL/stk_program.o \
./Core/Src/MCAL/tim.o \
./Core/Src/MCAL/usart_program.o 

C_DEPS += \
./Core/Src/MCAL/adc_program.d \
./Core/Src/MCAL/gpio_program.d \
./Core/Src/MCAL/i2c_program.d \
./Core/Src/MCAL/nvic_program.d \
./Core/Src/MCAL/rcc_program.d \
./Core/Src/MCAL/stk_program.d \
./Core/Src/MCAL/tim.d \
./Core/Src/MCAL/usart_program.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/MCAL/%.o Core/Src/MCAL/%.su Core/Src/MCAL/%.cyclo: ../Core/Src/MCAL/%.c Core/Src/MCAL/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Core/Inc/SL -I../Core/Inc/SL/MQTT -I../Core/Inc/MCAL -I../Core/Inc/MCAL/ADC -I../Core/Inc/MCAL/GPIO -I../Core/Inc/MCAL/NVIC -I../Core/Inc/MCAL/RCC -I../Core/Inc/MCAL/STK -I../Core/Inc/MCAL/USART -I../Core/Inc/HAL -I../Core/Inc/HAL/ESP -I../Core/Inc/HAL/MQ6 -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc/MCAL/I2C -I../Core/Inc/HAL/BMP280 -I../Core/Inc/MCAL/TIM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-MCAL

clean-Core-2f-Src-2f-MCAL:
	-$(RM) ./Core/Src/MCAL/adc_program.cyclo ./Core/Src/MCAL/adc_program.d ./Core/Src/MCAL/adc_program.o ./Core/Src/MCAL/adc_program.su ./Core/Src/MCAL/gpio_program.cyclo ./Core/Src/MCAL/gpio_program.d ./Core/Src/MCAL/gpio_program.o ./Core/Src/MCAL/gpio_program.su ./Core/Src/MCAL/i2c_program.cyclo ./Core/Src/MCAL/i2c_program.d ./Core/Src/MCAL/i2c_program.o ./Core/Src/MCAL/i2c_program.su ./Core/Src/MCAL/nvic_program.cyclo ./Core/Src/MCAL/nvic_program.d ./Core/Src/MCAL/nvic_program.o ./Core/Src/MCAL/nvic_program.su ./Core/Src/MCAL/rcc_program.cyclo ./Core/Src/MCAL/rcc_program.d ./Core/Src/MCAL/rcc_program.o ./Core/Src/MCAL/rcc_program.su ./Core/Src/MCAL/stk_program.cyclo ./Core/Src/MCAL/stk_program.d ./Core/Src/MCAL/stk_program.o ./Core/Src/MCAL/stk_program.su ./Core/Src/MCAL/tim.cyclo ./Core/Src/MCAL/tim.d ./Core/Src/MCAL/tim.o ./Core/Src/MCAL/tim.su ./Core/Src/MCAL/usart_program.cyclo ./Core/Src/MCAL/usart_program.d ./Core/Src/MCAL/usart_program.o ./Core/Src/MCAL/usart_program.su

.PHONY: clean-Core-2f-Src-2f-MCAL

