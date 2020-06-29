################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../broker.c \
../helper.c \
../listas.c \
../memory_routines.c \
../receive.c \
../routines.c \
../suscription.c 

OBJS += \
./broker.o \
./helper.o \
./listas.o \
./memory_routines.o \
./receive.o \
./routines.o \
./suscription.o 

C_DEPS += \
./broker.d \
./helper.d \
./listas.d \
./memory_routines.d \
./receive.d \
./routines.d \
./suscription.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/operating_systems/development/utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


