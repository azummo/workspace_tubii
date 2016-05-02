################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../contrib/pg_stat_statements/pg_stat_statements.c 

OBJS += \
./contrib/pg_stat_statements/pg_stat_statements.o 

C_DEPS += \
./contrib/pg_stat_statements/pg_stat_statements.d 


# Each subdirectory must supply rules for building sources it contributes
contrib/pg_stat_statements/%.o: ../contrib/pg_stat_statements/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


