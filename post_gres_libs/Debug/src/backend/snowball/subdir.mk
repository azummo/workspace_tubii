################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/backend/snowball/dict_snowball.c 

OBJS += \
./src/backend/snowball/dict_snowball.o 

C_DEPS += \
./src/backend/snowball/dict_snowball.d 


# Each subdirectory must supply rules for building sources it contributes
src/backend/snowball/%.o: ../src/backend/snowball/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


