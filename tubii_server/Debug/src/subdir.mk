################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ae.c \
../src/ae_epoll.c \
../src/anet.c \
../src/consumer.c \
../src/data.c \
../src/logging.c \
../src/producer.c \
../src/sds.c \
../src/sock.c \
../src/tubii-server.c \
../src/tubii_client.c 

OBJS += \
./src/ae.o \
./src/ae_epoll.o \
./src/anet.o \
./src/consumer.o \
./src/data.o \
./src/logging.o \
./src/producer.o \
./src/sds.o \
./src/sock.o \
./src/tubii-server.o \
./src/tubii_client.o 

C_DEPS += \
./src/ae.d \
./src/ae_epoll.d \
./src/anet.d \
./src/consumer.d \
./src/data.d \
./src/logging.d \
./src/producer.d \
./src/sds.d \
./src/sock.d \
./src/tubii-server.d \
./src/tubii_client.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Linux gcc compiler'
	arm-xilinx-linux-gnueabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


