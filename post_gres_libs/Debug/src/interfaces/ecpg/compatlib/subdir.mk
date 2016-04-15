################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/interfaces/ecpg/compatlib/informix.c 

OBJS += \
./src/interfaces/ecpg/compatlib/informix.o 

C_DEPS += \
./src/interfaces/ecpg/compatlib/informix.d 


# Each subdirectory must supply rules for building sources it contributes
src/interfaces/ecpg/compatlib/%.o: ../src/interfaces/ecpg/compatlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


