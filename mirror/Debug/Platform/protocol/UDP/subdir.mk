################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Platform/protocol/UDP/udp.c 

C_DEPS += \
./Platform/protocol/UDP/udp.d 

OBJS += \
./Platform/protocol/UDP/udp.o 


# Each subdirectory must supply rules for building sources it contributes
Platform/protocol/UDP/%.o: ../Platform/protocol/UDP/%.c Platform/protocol/UDP/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/tct/eclipse-workspace/mirror/App" -I/usr/include/modbus -I"/home/tct/eclipse-workspace/mirror/Common" -I"/home/tct/eclipse-workspace/mirror/Platform" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Platform-2f-protocol-2f-UDP

clean-Platform-2f-protocol-2f-UDP:
	-$(RM) ./Platform/protocol/UDP/udp.d ./Platform/protocol/UDP/udp.o

.PHONY: clean-Platform-2f-protocol-2f-UDP

