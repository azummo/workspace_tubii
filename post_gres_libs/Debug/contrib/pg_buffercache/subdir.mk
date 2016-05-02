################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../contrib/pg_buffercache/pg_buffercache_pages.c 

OBJS += \
./contrib/pg_buffercache/pg_buffercache_pages.o 

C_DEPS += \
./contrib/pg_buffercache/pg_buffercache_pages.d 


# Each subdirectory must supply rules for building sources it contributes
contrib/pg_buffercache/%.o: ../contrib/pg_buffercache/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


