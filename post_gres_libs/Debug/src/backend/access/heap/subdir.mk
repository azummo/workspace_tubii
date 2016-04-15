################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/backend/access/heap/heapam.c \
../src/backend/access/heap/hio.c \
../src/backend/access/heap/pruneheap.c \
../src/backend/access/heap/rewriteheap.c \
../src/backend/access/heap/syncscan.c \
../src/backend/access/heap/tuptoaster.c \
../src/backend/access/heap/visibilitymap.c 

OBJS += \
./src/backend/access/heap/heapam.o \
./src/backend/access/heap/hio.o \
./src/backend/access/heap/pruneheap.o \
./src/backend/access/heap/rewriteheap.o \
./src/backend/access/heap/syncscan.o \
./src/backend/access/heap/tuptoaster.o \
./src/backend/access/heap/visibilitymap.o 

C_DEPS += \
./src/backend/access/heap/heapam.d \
./src/backend/access/heap/hio.d \
./src/backend/access/heap/pruneheap.d \
./src/backend/access/heap/rewriteheap.d \
./src/backend/access/heap/syncscan.d \
./src/backend/access/heap/tuptoaster.d \
./src/backend/access/heap/visibilitymap.d 


# Each subdirectory must supply rules for building sources it contributes
src/backend/access/heap/%.o: ../src/backend/access/heap/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


