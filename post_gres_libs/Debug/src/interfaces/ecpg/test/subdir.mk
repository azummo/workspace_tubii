################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/interfaces/ecpg/test/pg_regress_ecpg.c 

OBJS += \
./src/interfaces/ecpg/test/pg_regress_ecpg.o 

C_DEPS += \
./src/interfaces/ecpg/test/pg_regress_ecpg.d 


# Each subdirectory must supply rules for building sources it contributes
src/interfaces/ecpg/test/%.o: ../src/interfaces/ecpg/test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


