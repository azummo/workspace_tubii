################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/backend/foreign/foreign.c 

OBJS += \
./src/backend/foreign/foreign.o 

C_DEPS += \
./src/backend/foreign/foreign.d 


# Each subdirectory must supply rules for building sources it contributes
src/backend/foreign/%.o: ../src/backend/foreign/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


