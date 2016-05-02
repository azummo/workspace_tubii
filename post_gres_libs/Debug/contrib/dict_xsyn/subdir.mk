################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../contrib/dict_xsyn/dict_xsyn.c 

OBJS += \
./contrib/dict_xsyn/dict_xsyn.o 

C_DEPS += \
./contrib/dict_xsyn/dict_xsyn.d 


# Each subdirectory must supply rules for building sources it contributes
contrib/dict_xsyn/%.o: ../contrib/dict_xsyn/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


