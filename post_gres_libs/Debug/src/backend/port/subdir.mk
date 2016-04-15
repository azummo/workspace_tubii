################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/backend/port/ipc_test.c \
../src/backend/port/posix_sema.c \
../src/backend/port/sysv_sema.c \
../src/backend/port/sysv_shmem.c \
../src/backend/port/win32_sema.c \
../src/backend/port/win32_shmem.c 

OBJS += \
./src/backend/port/ipc_test.o \
./src/backend/port/posix_sema.o \
./src/backend/port/sysv_sema.o \
./src/backend/port/sysv_shmem.o \
./src/backend/port/win32_sema.o \
./src/backend/port/win32_shmem.o 

C_DEPS += \
./src/backend/port/ipc_test.d \
./src/backend/port/posix_sema.d \
./src/backend/port/sysv_sema.d \
./src/backend/port/sysv_shmem.d \
./src/backend/port/win32_sema.d \
./src/backend/port/win32_shmem.d 


# Each subdirectory must supply rules for building sources it contributes
src/backend/port/%.o: ../src/backend/port/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


