################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../file_system_routines.c \
../gamecard.c \
../helper.c \
../messages.c 

OBJS += \
./file_system_routines.o \
./gamecard.o \
./helper.o \
./messages.o 

C_DEPS += \
./file_system_routines.d \
./gamecard.d \
./helper.d \
./messages.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/apps/tp-2020-1c-Skynet/utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


