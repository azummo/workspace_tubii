################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../contrib/dblink/dblink.c 

OBJS += \
./contrib/dblink/dblink.o 

C_DEPS += \
./contrib/dblink/dblink.d 


# Each subdirectory must supply rules for building sources it contributes
contrib/dblink/%.o: ../contrib/dblink/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


