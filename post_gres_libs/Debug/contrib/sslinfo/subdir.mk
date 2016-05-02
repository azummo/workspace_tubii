################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../contrib/sslinfo/sslinfo.c 

OBJS += \
./contrib/sslinfo/sslinfo.o 

C_DEPS += \
./contrib/sslinfo/sslinfo.d 


# Each subdirectory must supply rules for building sources it contributes
contrib/sslinfo/%.o: ../contrib/sslinfo/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


