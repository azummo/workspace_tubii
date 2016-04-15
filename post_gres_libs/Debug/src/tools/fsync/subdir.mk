################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/tools/fsync/test_fsync.c 

OBJS += \
./src/tools/fsync/test_fsync.o 

C_DEPS += \
./src/tools/fsync/test_fsync.d 


# Each subdirectory must supply rules for building sources it contributes
src/tools/fsync/%.o: ../src/tools/fsync/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


