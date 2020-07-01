################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../team.c \
../team_structs.c \
../utilities.c 

OBJS += \
./team.o \
./team_structs.o \
./utilities.o 

C_DEPS += \
./team.d \
./team_structs.d \
./utilities.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Ipthread -Icommons -I"/home/utnso/apps/tp-2020-1c-Skynet/utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


