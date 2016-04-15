################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/test/locale/test-ctype.c 

OBJS += \
./src/test/locale/test-ctype.o 

C_DEPS += \
./src/test/locale/test-ctype.d 


# Each subdirectory must supply rules for building sources it contributes
src/test/locale/%.o: ../src/test/locale/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


