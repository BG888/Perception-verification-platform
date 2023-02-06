################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/CommandResolve/CommandCB.c \
../App/CommandResolve/CommandResolve.c 

C_DEPS += \
./App/CommandResolve/CommandCB.d \
./App/CommandResolve/CommandResolve.d 

OBJS += \
./App/CommandResolve/CommandCB.o \
./App/CommandResolve/CommandResolve.o 


# Each subdirectory must supply rules for building sources it contributes
App/CommandResolve/%.o: ../App/CommandResolve/%.c App/CommandResolve/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/tct/eclipse-workspace/mirror/App" -I/usr/include/modbus -I"/home/tct/eclipse-workspace/mirror/Common" -I"/home/tct/eclipse-workspace/mirror/Platform" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-App-2f-CommandResolve

clean-App-2f-CommandResolve:
	-$(RM) ./App/CommandResolve/CommandCB.d ./App/CommandResolve/CommandCB.o ./App/CommandResolve/CommandResolve.d ./App/CommandResolve/CommandResolve.o

.PHONY: clean-App-2f-CommandResolve

