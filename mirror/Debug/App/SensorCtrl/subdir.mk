################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/SensorCtrl/DeviceCtrl.c \
../App/SensorCtrl/Locker4G.c \
../App/SensorCtrl/SceneElement.c \
../App/SensorCtrl/Transaction.c 

C_DEPS += \
./App/SensorCtrl/DeviceCtrl.d \
./App/SensorCtrl/Locker4G.d \
./App/SensorCtrl/SceneElement.d \
./App/SensorCtrl/Transaction.d 

OBJS += \
./App/SensorCtrl/DeviceCtrl.o \
./App/SensorCtrl/Locker4G.o \
./App/SensorCtrl/SceneElement.o \
./App/SensorCtrl/Transaction.o 


# Each subdirectory must supply rules for building sources it contributes
App/SensorCtrl/%.o: ../App/SensorCtrl/%.c App/SensorCtrl/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/tct/eclipse-workspace/mirror/App" -I/usr/include/modbus -I"/home/tct/eclipse-workspace/mirror/Common" -I"/home/tct/eclipse-workspace/mirror/Platform" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-App-2f-SensorCtrl

clean-App-2f-SensorCtrl:
	-$(RM) ./App/SensorCtrl/DeviceCtrl.d ./App/SensorCtrl/DeviceCtrl.o ./App/SensorCtrl/Locker4G.d ./App/SensorCtrl/Locker4G.o ./App/SensorCtrl/SceneElement.d ./App/SensorCtrl/SceneElement.o ./App/SensorCtrl/Transaction.d ./App/SensorCtrl/Transaction.o

.PHONY: clean-App-2f-SensorCtrl

