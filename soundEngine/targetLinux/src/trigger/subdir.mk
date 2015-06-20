################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/trigger/ManualTrigger.cpp \
../src/trigger/ShapeTrigger.cpp \
../src/trigger/SoundTrigger.cpp 

OBJS += \
./src/trigger/ManualTrigger.o \
./src/trigger/ShapeTrigger.o \
./src/trigger/SoundTrigger.o 

CPP_DEPS += \
./src/trigger/ManualTrigger.d \
./src/trigger/ShapeTrigger.d \
./src/trigger/SoundTrigger.d 


# Each subdirectory must supply rules for building sources it contributes
src/trigger/%.o: ../src/trigger/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -DD_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/soundEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


