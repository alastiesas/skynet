################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/conexion.c \
../libs/mensajes.c \
../libs/serialize.c 

OBJS += \
./libs/conexion.o \
./libs/mensajes.o \
./libs/serialize.o 

C_DEPS += \
./libs/conexion.d \
./libs/mensajes.d \
./libs/serialize.d 


# Each subdirectory must supply rules for building sources it contributes
libs/%.o: ../libs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


