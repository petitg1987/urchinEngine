################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/resources/sound/SceneSound.cpp \
../src/resources/sound/SoundReaderWriter.cpp \
../src/resources/sound/SoundTriggerReaderWriter.cpp 

OBJS += \
./src/resources/sound/SceneSound.o \
./src/resources/sound/SoundReaderWriter.o \
./src/resources/sound/SoundTriggerReaderWriter.o 

CPP_DEPS += \
./src/resources/sound/SceneSound.d \
./src/resources/sound/SoundReaderWriter.d \
./src/resources/sound/SoundTriggerReaderWriter.d 


# Each subdirectory must supply rules for building sources it contributes
src/resources/sound/%.o: ../src/resources/sound/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/soundEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/3dEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/mapHandler/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


