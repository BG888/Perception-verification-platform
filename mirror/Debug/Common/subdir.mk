################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Common/CommonMemory.c \
../Common/EndianChange.c \
../Common/LinkList.c \
../Common/crc32.c 

C_DEPS += \
./Common/CommonMemory.d \
./Common/EndianChange.d \
./Common/LinkList.d \
./Common/crc32.d 

OBJS += \
./Common/CommonMemory.o \
./Common/EndianChange.o \
./Common/LinkList.o \
./Common/crc32.o 


# Each subdirectory must supply rules for building sources it contributes
Common/%.o: ../Common/%.c Common/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/tct/eclipse-workspace/mirror/App" -I/usr/include/modbus -I"/home/tct/eclipse-workspace/mirror/Common" -I"/home/tct/eclipse-workspace/mirror/Platform" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Common

clean-Common:
	-$(RM) ./Common/CommonMemory.d ./Common/CommonMemory.o ./Common/EndianChange.d ./Common/EndianChange.o ./Common/LinkList.d ./Common/LinkList.o ./Common/crc32.d ./Common/crc32.o

.PHONY: clean-Common

