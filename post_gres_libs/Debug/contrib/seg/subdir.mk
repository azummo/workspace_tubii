################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../contrib/seg/seg.c \
../contrib/seg/segparse.c \
../contrib/seg/segscan.c 

OBJS += \
./contrib/seg/seg.o \
./contrib/seg/segparse.o \
./contrib/seg/segscan.o 

C_DEPS += \
./contrib/seg/seg.d \
./contrib/seg/segparse.d \
./contrib/seg/segscan.d 


# Each subdirectory must supply rules for building sources it contributes
contrib/seg/%.o: ../contrib/seg/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


