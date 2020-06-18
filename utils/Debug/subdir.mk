################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../conexion.c \
../mensajes.c \
../serialize.c \
../structs.c \
../utils.c \
../utils2.c 

OBJS += \
./conexion.o \
./mensajes.o \
./serialize.o \
./structs.o \
./utils.o \
./utils2.o 

C_DEPS += \
./conexion.d \
./mensajes.d \
./serialize.d \
./structs.d \
./utils.d \
./utils2.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


