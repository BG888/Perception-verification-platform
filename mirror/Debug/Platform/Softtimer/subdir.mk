################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Platform/Softtimer/softtime.c 

C_DEPS += \
./Platform/Softtimer/softtime.d 

OBJS += \
./Platform/Softtimer/softtime.o 


# Each subdirectory must supply rules for building sources it contributes
Platform/Softtimer/%.o: ../Platform/Softtimer/%.c Platform/Softtimer/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/tct/eclipse-workspace/mirror/App" -I/usr/include/modbus -I"/home/tct/eclipse-workspace/mirror/Common" -I"/home/tct/eclipse-workspace/mirror/Platform" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Platform-2f-Softtimer

clean-Platform-2f-Softtimer:
	-$(RM) ./Platform/Softtimer/softtime.d ./Platform/Softtimer/softtime.o

.PHONY: clean-Platform-2f-Softtimer

