################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.c 

OBJS += \
./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.o 

C_DEPS += \
./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.d 


# Each subdirectory must supply rules for building sources it contributes
Core/.metadata/.plugins/org.eclipse.cdt.make.core/%.o Core/.metadata/.plugins/org.eclipse.cdt.make.core/%.su Core/.metadata/.plugins/org.eclipse.cdt.make.core/%.cyclo: ../Core/.metadata/.plugins/org.eclipse.cdt.make.core/%.c Core/.metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Core/Inc/SL -I../Core/Inc/SL/MQTT -I../Core/Inc/MCAL -I../Core/Inc/MCAL/ADC -I../Core/Inc/MCAL/GPIO -I../Core/Inc/MCAL/NVIC -I../Core/Inc/MCAL/RCC -I../Core/Inc/MCAL/STK -I../Core/Inc/MCAL/USART -I../Core/Inc/HAL -I../Core/Inc/HAL/ESP -I../Core/Inc/HAL/MQ6 -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc/MCAL/I2C -I../Core/Inc/HAL/BMP280 -I../Core/Inc/MCAL/TIM -I../Core/Inc/MCAL/IWDG -I../Core/Inc/drivers/peripheral -I../Core/Inc/drivers/device -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f--2e-metadata-2f--2e-plugins-2f-org-2e-eclipse-2e-cdt-2e-make-2e-core

clean-Core-2f--2e-metadata-2f--2e-plugins-2f-org-2e-eclipse-2e-cdt-2e-make-2e-core:
	-$(RM) ./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.cyclo ./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.d ./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.o ./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.su

.PHONY: clean-Core-2f--2e-metadata-2f--2e-plugins-2f-org-2e-eclipse-2e-cdt-2e-make-2e-core

