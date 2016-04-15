################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/backend/rewrite/rewriteDefine.c \
../src/backend/rewrite/rewriteHandler.c \
../src/backend/rewrite/rewriteManip.c \
../src/backend/rewrite/rewriteRemove.c \
../src/backend/rewrite/rewriteSupport.c 

OBJS += \
./src/backend/rewrite/rewriteDefine.o \
./src/backend/rewrite/rewriteHandler.o \
./src/backend/rewrite/rewriteManip.o \
./src/backend/rewrite/rewriteRemove.o \
./src/backend/rewrite/rewriteSupport.o 

C_DEPS += \
./src/backend/rewrite/rewriteDefine.d \
./src/backend/rewrite/rewriteHandler.d \
./src/backend/rewrite/rewriteManip.d \
./src/backend/rewrite/rewriteRemove.d \
./src/backend/rewrite/rewriteSupport.d 


# Each subdirectory must supply rules for building sources it contributes
src/backend/rewrite/%.o: ../src/backend/rewrite/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


