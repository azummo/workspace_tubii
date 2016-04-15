################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/backend/port/darwin/system.c 

OBJS += \
./src/backend/port/darwin/system.o 

C_DEPS += \
./src/backend/port/darwin/system.d 


# Each subdirectory must supply rules for building sources it contributes
src/backend/port/darwin/%.o: ../src/backend/port/darwin/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


