################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/collision/island/IslandContainer.cpp \
../src/collision/island/IslandElement.cpp \
../src/collision/island/IslandElementLink.cpp \
../src/collision/island/IslandManager.cpp 

OBJS += \
./src/collision/island/IslandContainer.o \
./src/collision/island/IslandElement.o \
./src/collision/island/IslandElementLink.o \
./src/collision/island/IslandManager.o 

CPP_DEPS += \
./src/collision/island/IslandContainer.d \
./src/collision/island/IslandElement.d \
./src/collision/island/IslandElementLink.d \
./src/collision/island/IslandManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/collision/island/%.o: ../src/collision/island/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


