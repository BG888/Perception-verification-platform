################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Platform/protocol/TCP/TCPClient.c \
../Platform/protocol/TCP/TCPServer.c 

C_DEPS += \
./Platform/protocol/TCP/TCPClient.d \
./Platform/protocol/TCP/TCPServer.d 

OBJS += \
./Platform/protocol/TCP/TCPClient.o \
./Platform/protocol/TCP/TCPServer.o 


# Each subdirectory must supply rules for building sources it contributes
Platform/protocol/TCP/%.o: ../Platform/protocol/TCP/%.c Platform/protocol/TCP/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/tct/eclipse-workspace/mirror/App" -I/usr/include/modbus -I"/home/tct/eclipse-workspace/mirror/Common" -I"/home/tct/eclipse-workspace/mirror/Platform" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Platform-2f-protocol-2f-TCP

clean-Platform-2f-protocol-2f-TCP:
	-$(RM) ./Platform/protocol/TCP/TCPClient.d ./Platform/protocol/TCP/TCPClient.o ./Platform/protocol/TCP/TCPServer.d ./Platform/protocol/TCP/TCPServer.o

.PHONY: clean-Platform-2f-protocol-2f-TCP

