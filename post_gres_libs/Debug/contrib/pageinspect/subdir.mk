################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../contrib/pageinspect/btreefuncs.c \
../contrib/pageinspect/fsmfuncs.c \
../contrib/pageinspect/heapfuncs.c \
../contrib/pageinspect/rawpage.c 

OBJS += \
./contrib/pageinspect/btreefuncs.o \
./contrib/pageinspect/fsmfuncs.o \
./contrib/pageinspect/heapfuncs.o \
./contrib/pageinspect/rawpage.o 

C_DEPS += \
./contrib/pageinspect/btreefuncs.d \
./contrib/pageinspect/fsmfuncs.d \
./contrib/pageinspect/heapfuncs.d \
./contrib/pageinspect/rawpage.d 


# Each subdirectory must supply rules for building sources it contributes
contrib/pageinspect/%.o: ../contrib/pageinspect/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


