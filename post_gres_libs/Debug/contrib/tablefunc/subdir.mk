################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../contrib/tablefunc/tablefunc.c 

OBJS += \
./contrib/tablefunc/tablefunc.o 

C_DEPS += \
./contrib/tablefunc/tablefunc.d 


# Each subdirectory must supply rules for building sources it contributes
contrib/tablefunc/%.o: ../contrib/tablefunc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


