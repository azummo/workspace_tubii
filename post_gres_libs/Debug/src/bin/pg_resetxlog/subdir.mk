################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bin/pg_resetxlog/pg_resetxlog.c 

OBJS += \
./src/bin/pg_resetxlog/pg_resetxlog.o 

C_DEPS += \
./src/bin/pg_resetxlog/pg_resetxlog.d 


# Each subdirectory must supply rules for building sources it contributes
src/bin/pg_resetxlog/%.o: ../src/bin/pg_resetxlog/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


