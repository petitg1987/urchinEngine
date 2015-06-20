################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/behavior/SmoothStopAction.cpp \
../src/behavior/SoundBehavior.cpp 

OBJS += \
./src/behavior/SmoothStopAction.o \
./src/behavior/SoundBehavior.o 

CPP_DEPS += \
./src/behavior/SmoothStopAction.d \
./src/behavior/SoundBehavior.d 


# Each subdirectory must supply rules for building sources it contributes
src/behavior/%.o: ../src/behavior/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -DD_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/soundEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


