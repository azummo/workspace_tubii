################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/backend/utils/resowner/resowner.c 

OBJS += \
./src/backend/utils/resowner/resowner.o 

C_DEPS += \
./src/backend/utils/resowner/resowner.d 


# Each subdirectory must supply rules for building sources it contributes
src/backend/utils/resowner/%.o: ../src/backend/utils/resowner/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


