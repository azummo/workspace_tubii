################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/adlist.c \
../src/ae.c \
../src/ae_epoll.c \
../src/anet.c \
../src/async.c \
../src/data.c \
../src/dict.c \
../src/hiredis.c \
../src/logging.c \
../src/net.c \
../src/read.c \
../src/sds.c \
../src/server.c \
../src/sha1.c \
../src/sock.c \
../src/tubii-server.c \
../src/tubii_client.c \
../src/util.c 

OBJS += \
./src/adlist.o \
./src/ae.o \
./src/ae_epoll.o \
./src/anet.o \
./src/async.o \
./src/data.o \
./src/dict.o \
./src/hiredis.o \
./src/logging.o \
./src/net.o \
./src/read.o \
./src/sds.o \
./src/server.o \
./src/sha1.o \
./src/sock.o \
./src/tubii-server.o \
./src/tubii_client.o \
./src/util.o 

C_DEPS += \
./src/adlist.d \
./src/ae.d \
./src/ae_epoll.d \
./src/anet.d \
./src/async.d \
./src/data.d \
./src/dict.d \
./src/hiredis.d \
./src/logging.d \
./src/net.d \
./src/read.d \
./src/sds.d \
./src/server.d \
./src/sha1.d \
./src/sock.d \
./src/tubii-server.d \
./src/tubii_client.d \
./src/util.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Linux gcc compiler'
	arm-xilinx-linux-gnueabi-gcc -Wall -O0 -g3 -I"C:\Users\Ian\Documents\GitHub\workspace_tubii\TestLib\src" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


