################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../contrib/pgstattuple/pgstatindex.c \
../contrib/pgstattuple/pgstattuple.c 

OBJS += \
./contrib/pgstattuple/pgstatindex.o \
./contrib/pgstattuple/pgstattuple.o 

C_DEPS += \
./contrib/pgstattuple/pgstatindex.d \
./contrib/pgstattuple/pgstattuple.d 


# Each subdirectory must supply rules for building sources it contributes
contrib/pgstattuple/%.o: ../contrib/pgstattuple/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


