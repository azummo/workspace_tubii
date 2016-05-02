################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../contrib/pg_trgm/trgm_gin.c \
../contrib/pg_trgm/trgm_gist.c \
../contrib/pg_trgm/trgm_op.c 

OBJS += \
./contrib/pg_trgm/trgm_gin.o \
./contrib/pg_trgm/trgm_gist.o \
./contrib/pg_trgm/trgm_op.o 

C_DEPS += \
./contrib/pg_trgm/trgm_gin.d \
./contrib/pg_trgm/trgm_gist.d \
./contrib/pg_trgm/trgm_op.d 


# Each subdirectory must supply rules for building sources it contributes
contrib/pg_trgm/%.o: ../contrib/pg_trgm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


