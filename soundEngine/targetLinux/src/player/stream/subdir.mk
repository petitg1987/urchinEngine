################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/player/stream/AudioStreamPlayer.cpp \
../src/player/stream/StreamChunk.cpp \
../src/player/stream/StreamUpdateTask.cpp \
../src/player/stream/StreamUpdateWorker.cpp 

OBJS += \
./src/player/stream/AudioStreamPlayer.o \
./src/player/stream/StreamChunk.o \
./src/player/stream/StreamUpdateTask.o \
./src/player/stream/StreamUpdateWorker.o 

CPP_DEPS += \
./src/player/stream/AudioStreamPlayer.d \
./src/player/stream/StreamChunk.d \
./src/player/stream/StreamUpdateTask.d \
./src/player/stream/StreamUpdateWorker.d 


# Each subdirectory must supply rules for building sources it contributes
src/player/stream/%.o: ../src/player/stream/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -DD_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/soundEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


