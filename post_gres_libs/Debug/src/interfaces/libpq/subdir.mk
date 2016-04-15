################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/interfaces/libpq/fe-auth.c \
../src/interfaces/libpq/fe-connect.c \
../src/interfaces/libpq/fe-exec.c \
../src/interfaces/libpq/fe-lobj.c \
../src/interfaces/libpq/fe-misc.c \
../src/interfaces/libpq/fe-print.c \
../src/interfaces/libpq/fe-protocol2.c \
../src/interfaces/libpq/fe-protocol3.c \
../src/interfaces/libpq/fe-secure.c \
../src/interfaces/libpq/libpq-events.c \
../src/interfaces/libpq/pqexpbuffer.c \
../src/interfaces/libpq/pqsignal.c \
../src/interfaces/libpq/pthread-win32.c \
../src/interfaces/libpq/win32.c 

OBJS += \
./src/interfaces/libpq/fe-auth.o \
./src/interfaces/libpq/fe-connect.o \
./src/interfaces/libpq/fe-exec.o \
./src/interfaces/libpq/fe-lobj.o \
./src/interfaces/libpq/fe-misc.o \
./src/interfaces/libpq/fe-print.o \
./src/interfaces/libpq/fe-protocol2.o \
./src/interfaces/libpq/fe-protocol3.o \
./src/interfaces/libpq/fe-secure.o \
./src/interfaces/libpq/libpq-events.o \
./src/interfaces/libpq/pqexpbuffer.o \
./src/interfaces/libpq/pqsignal.o \
./src/interfaces/libpq/pthread-win32.o \
./src/interfaces/libpq/win32.o 

C_DEPS += \
./src/interfaces/libpq/fe-auth.d \
./src/interfaces/libpq/fe-connect.d \
./src/interfaces/libpq/fe-exec.d \
./src/interfaces/libpq/fe-lobj.d \
./src/interfaces/libpq/fe-misc.d \
./src/interfaces/libpq/fe-print.d \
./src/interfaces/libpq/fe-protocol2.d \
./src/interfaces/libpq/fe-protocol3.d \
./src/interfaces/libpq/fe-secure.d \
./src/interfaces/libpq/libpq-events.d \
./src/interfaces/libpq/pqexpbuffer.d \
./src/interfaces/libpq/pqsignal.d \
./src/interfaces/libpq/pthread-win32.d \
./src/interfaces/libpq/win32.d 


# Each subdirectory must supply rules for building sources it contributes
src/interfaces/libpq/%.o: ../src/interfaces/libpq/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


