################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/trigger/shape/SoundBox.cpp \
../src/trigger/shape/SoundShape.cpp \
../src/trigger/shape/SoundSphere.cpp 

OBJS += \
./src/trigger/shape/SoundBox.o \
./src/trigger/shape/SoundShape.o \
./src/trigger/shape/SoundSphere.o 

CPP_DEPS += \
./src/trigger/shape/SoundBox.d \
./src/trigger/shape/SoundShape.d \
./src/trigger/shape/SoundSphere.d 


# Each subdirectory must supply rules for building sources it contributes
src/trigger/shape/%.o: ../src/trigger/shape/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -DD_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/soundEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


