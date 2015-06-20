################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/player/AudioPlayer.cpp 

OBJS += \
./src/player/AudioPlayer.o 

CPP_DEPS += \
./src/player/AudioPlayer.d 


# Each subdirectory must supply rules for building sources it contributes
src/player/%.o: ../src/player/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -DD_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/soundEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


